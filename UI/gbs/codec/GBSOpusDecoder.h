// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GBS_OPUS_DECODER_H
#define GBS_OPUS_DECODER_H

#include <memory>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}


#include "GBSAudioCodec.h"

class OpusDecoder {

public:
	explicit OpusDecoder(const FrameFormat format);
	virtual ~OpusDecoder();

	bool init();
	bool getAudioFrame(AudioFrame* audioFrame);
	void onFrame(const Frame& frame);

private:
	struct FFContext {
		AVCodecContext* decoder{nullptr};
		AVFrame* frame{nullptr};
		AVPacket packet{};
		AVAudioFifo* fifo{nullptr};
		AVFrame* outFrame{nullptr};
	};

	struct AudioConfig {
		AVSampleFormat format{AV_SAMPLE_FMT_S16};
		int sampleRate{48000};
		int channels{2};
		uint64_t channelLayout{AV_CH_LAYOUT_STEREO};
	};

	bool initDecoder();
	bool initAudioFifo();
	bool processDecodedFrame(AVFrame* frame);
	void deliverAudioFrame(const uint8_t* data, int samples);
	char* getErrorStr(int err);

	std::mutex m_mutex;
	bool m_initialized{false};
	
	FFContext m_ff;
	AudioConfig m_inputConfig;
	AudioConfig m_outputConfig;
	
	int64_t m_timestamp{0};
	char m_errorBuf[AV_ERROR_MAX_STRING_SIZE];

	FrameFormat m_inputFormat;
	FrameFormat m_outputFormat{FRAME_FORMAT_PCM_48000_2};

	std::thread m_decodeThread;
	std::queue<Frame> m_frameQueue;
	std::condition_variable m_cv;
	bool m_running{false};

	void decodeThreadFunc();
	void startDecodeThread();
	void stopDecodeThread();
};

#endif // GBS_OPUS_DECODER_H
