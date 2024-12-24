// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0


#include "GBSAudioCodec.h"
#include "GBSOpusDecoder.h"

#include <QDebug>


OpusDecoder::OpusDecoder(const FrameFormat format)
	: m_inputFormat(format) {
	qDebug() << "Creating FFmpeg Opus Decoder";
	av_log_set_level(AV_LOG_WARNING);
}

OpusDecoder::~OpusDecoder() {
	stopDecodeThread();
	
	std::lock_guard<std::mutex> lock(m_mutex);
	
	if (m_ff.fifo) {
		av_audio_fifo_free(m_ff.fifo);
	}
	if (m_ff.outFrame) {
		av_frame_free(&m_ff.outFrame);
	}
	if (m_ff.frame) {
		av_frame_free(&m_ff.frame);
	}
	if (m_ff.decoder) {
		avcodec_free_context(&m_ff.decoder);
	}
}

bool OpusDecoder::init() {
	std::lock_guard<std::mutex> lock(m_mutex);
	
	if (m_initialized) {
		return true;
	}

	if (!initDecoder() || !initAudioFifo()) {
		qDebug() << "Failed to initialize decoder";
		return false;
	}
	
	m_initialized = true;
	startDecodeThread();
	return true;
}

void OpusDecoder::startDecodeThread() {
	m_running = true;
	m_decodeThread = std::thread(&OpusDecoder::decodeThreadFunc, this);
}

void OpusDecoder::stopDecodeThread() {
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_running = false;
	}
	m_cv.notify_one();
	
	if (m_decodeThread.joinable()) {
		m_decodeThread.join();
	}
}

void OpusDecoder::onFrame(const Frame& frame) {
	if (!m_initialized) {
		qDebug() << "Decoder not initialized";
		return;
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_frameQueue.push(frame);
	}
	m_cv.notify_one();
}

void OpusDecoder::decodeThreadFunc() {
	while (true) {
		Frame frame;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, [this]() {
				return !m_running || !m_frameQueue.empty();
			});
			
			if (!m_running && m_frameQueue.empty()) {
				break;
			}
			
			frame = m_frameQueue.front();
			m_frameQueue.pop();
		}

		// 处理解码
		if (frame.format != m_inputFormat) {
			qDebug() << "Invalid input format";
			continue;
		}

		// 准备packet并解码
		av_packet_unref(&m_ff.packet);
		m_ff.packet.data = frame.payload;
		m_ff.packet.size = (int)frame.length;

		int ret = avcodec_send_packet(m_ff.decoder, &m_ff.packet);
		if (ret < 0) {
			qDebug() << "Error sending packet:" << getErrorStr(ret);
			continue;
		}

		while (ret >= 0) {
			ret = avcodec_receive_frame(m_ff.decoder, m_ff.frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				break;
			} else if (ret < 0) {
				qDebug() << "Error receiving frame:" << getErrorStr(ret);
				break;
			}

			processDecodedFrame(m_ff.frame);
		}
	}
}

bool OpusDecoder::initDecoder() {
	const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
	if (!codec) {
		qDebug() << "Opus decoder not found";
		return false;
	}

	m_ff.decoder = avcodec_alloc_context3(codec);
	if (!m_ff.decoder) {
		qDebug() << "Could not allocate decoder context";
		return false;
	}

	// 设置解码器参数
	m_ff.decoder->sample_fmt = m_inputConfig.format;
	m_ff.decoder->sample_rate = m_inputConfig.sampleRate;
	
	// 使用新的通道布局 API
	av_channel_layout_default(&m_ff.decoder->ch_layout, m_inputConfig.channels);
	// 如果需要特定的通道布局，可以使用：
	// av_channel_layout_from_mask(&m_ff.decoder->ch_layout, m_inputConfig.channelLayout);

	int ret = avcodec_open2(m_ff.decoder, codec, nullptr);
	if (ret < 0) {
		qDebug() << "Could not open decoder:" << getErrorStr(ret);
		return false;
	}

	m_ff.frame = av_frame_alloc();
	if (!m_ff.frame) {
		qDebug() << "Could not allocate input frame";
		return false;
	}

	return true;
}

bool OpusDecoder::initAudioFifo() {
	m_ff.fifo = av_audio_fifo_alloc(
		m_outputConfig.format,
		m_outputConfig.channels,
		m_outputConfig.sampleRate / 50  // 20ms buffer
	);
	
	if (!m_ff.fifo) {
		qDebug() << "Could not allocate FIFO";
		return false;
	}

	m_ff.outFrame = av_frame_alloc();
	if (!m_ff.outFrame) {
		qDebug() << "Could not allocate output frame";
		return false;
	}

	m_ff.outFrame->format = m_outputConfig.format;
	// 使用新的通道布局 API
	av_channel_layout_default(&m_ff.outFrame->ch_layout, m_outputConfig.channels);
	// 如果需要特定的通道布局，可以使用：
	// av_channel_layout_from_mask(&m_ff.outFrame->ch_layout, m_outputConfig.channelLayout);
	m_ff.outFrame->sample_rate = m_outputConfig.sampleRate;
	m_ff.outFrame->nb_samples = m_outputConfig.sampleRate / 100;  // 10ms

	int ret = av_frame_get_buffer(m_ff.outFrame, 0);
	if (ret < 0) {
		qDebug() << "Could not allocate output frame buffer:" << getErrorStr(ret);
		return false;
	}

	return true;
}

bool OpusDecoder::processDecodedFrame(AVFrame* frame) {
	// 写入FIFO
	int ret = av_audio_fifo_write(m_ff.fifo, (void**)frame->data, frame->nb_samples);
	if (ret < frame->nb_samples) {
		qDebug() << "Could not write to FIFO";
		return false;
	}

	// 从FIFO读取固定大小的数据
	while (av_audio_fifo_size(m_ff.fifo) >= m_ff.outFrame->nb_samples) {
		ret = av_audio_fifo_read(m_ff.fifo, (void**)m_ff.outFrame->data, m_ff.outFrame->nb_samples);
		if (ret < m_ff.outFrame->nb_samples) {
			qDebug() << "Could not read from FIFO";
			return false;
		}

		deliverAudioFrame(m_ff.outFrame->data[0], m_ff.outFrame->nb_samples);
	}

	return true;
}

void OpusDecoder::deliverAudioFrame(const uint8_t* data, int samples) {
	Frame outFrame;
	//outFrame.format = m_outputFormat;
	outFrame.payload = const_cast<uint8_t*>(data);
	outFrame.length = samples * m_outputConfig.channels * sizeof(int16_t);
	
	outFrame.additionalInfo.audio.sampleRate = m_outputConfig.sampleRate;
	outFrame.additionalInfo.audio.channels = m_outputConfig.channels;
	outFrame.additionalInfo.audio.nbSamples = samples;
	
	
	outFrame.timeStamp = m_timestamp;

	m_timestamp += (samples * 1000) / m_outputConfig.sampleRate;
}

bool OpusDecoder::getAudioFrame(AudioFrame* audioFrame) {
	// 实现音频帧获取逻辑
	return false;
}

char* OpusDecoder::getErrorStr(int err) {
	av_strerror(err, m_errorBuf, AV_ERROR_MAX_STRING_SIZE);
	return m_errorBuf;
}

