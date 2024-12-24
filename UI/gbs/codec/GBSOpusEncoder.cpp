#include "GBSOpusEncoder.h"
#include <QDebug>

// FFmpeg 日志回调函数
void ffmpeg_log(void *ptr, int level, const char *fmt, va_list vl)
{
	if (level > AV_LOG_VERBOSE) {
		return;
	}
	// 将日志信息格式化为一个字符串
	char log_msg[1024];
	vsnprintf(log_msg, sizeof(log_msg), fmt, vl);
	
	// 根据日志级别输出日志
	switch (level) {
	case AV_LOG_FATAL:
	case AV_LOG_ERROR:
		qCritical() << "FFmpeg ERROR:" << log_msg;
		break;
	case AV_LOG_WARNING:
		qWarning() << "FFmpeg WARNING:" << log_msg;
		break;
	case AV_LOG_INFO:
		qInfo() << "FFmpeg INFO:" << log_msg;
		break;
	case AV_LOG_VERBOSE:
	case AV_LOG_DEBUG:
	case AV_LOG_TRACE:
		qDebug() << "FFmpeg DEBUG:" << log_msg;
		break;
	default:
		qDebug() << "FFmpeg LOG:" << log_msg;
		break;
	}
}

GBSOpusEncoder::GBSOpusEncoder()
    : m_valid(false)
    , m_channels(2)
    , m_sampleRate(48000)
    , m_audioEnc(nullptr)
    , m_audioFifo(nullptr)
    , m_audioFrame(nullptr)
    , m_formatCtx(nullptr)
    , m_stream(nullptr)
{
    // FFmpeg日志级别设置
	av_log_set_level(AV_LOG_VERBOSE);
    av_log_set_callback(ffmpeg_log);
}

GBSOpusEncoder::~GBSOpusEncoder()
{
    if (m_formatCtx) {
        // 写入文件尾
        if (m_valid && m_formatCtx->pb) {
            av_write_trailer(m_formatCtx);
        }
        
        // 关闭输出文件
        if (m_formatCtx->pb) {
            avio_closep(&m_formatCtx->pb);
        }
        
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
    }

    if (m_audioFrame) {
        av_frame_free(&m_audioFrame);
    }

    if (m_audioFifo) {
        av_audio_fifo_free(m_audioFifo);
    }

    if (m_audioEnc) {
        avcodec_free_context(&m_audioEnc);
    }
}

bool GBSOpusEncoder::init() {
	return initEncoder();
}
bool GBSOpusEncoder::initEncoder()
{
    int ret;
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_OPUS);
    if (!codec) {
        qDebug() << "Could not find OPUS encoder";
        return false;
    }
    const AVCodecDescriptor *codec_desc = avcodec_descriptor_get(codec->id);

    if (!codec_desc) {
		qDebug() <<"Failed to get codec descriptor";
	    return false;
    }

    m_audioEnc = avcodec_alloc_context3(codec);
    if (!m_audioEnc) {
        qDebug() << "Cannot allocate audio encoder context";
        return false;
    }

    // 明确指定立体声通道布局
    av_channel_layout_uninit(&m_audioEnc->ch_layout);  // 先清除默认布局
    if (m_channels == 2) {
        av_channel_layout_from_mask(&m_audioEnc->ch_layout, AV_CH_LAYOUT_STEREO);
    } else if (m_channels == 1) {
        av_channel_layout_from_mask(&m_audioEnc->ch_layout, AV_CH_LAYOUT_MONO);
    } else {
        qDebug() << "Unsupported channel count:" << m_channels;
        goto fail;
    }
    
    // 其他编码器参数
    m_audioEnc->sample_rate = m_sampleRate;
    m_audioEnc->sample_fmt = AV_SAMPLE_FMT_FLT;
    m_audioEnc->bit_rate = 128000;  // 对于立体声，可以考虑提高比特率
    
    m_audioEnc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    char buf[256];
    av_channel_layout_describe(&m_audioEnc->ch_layout, buf, 256);

    ret = avcodec_open2(m_audioEnc, codec, nullptr);
    if (ret < 0) {
        qDebug() << "Cannot open OPUS encoder:" << ff_err2str(ret);
        goto fail;
    }

    m_audioFifo = av_audio_fifo_alloc(m_audioEnc->sample_fmt, m_channels, m_audioEnc->frame_size);
    if (!m_audioFifo) {
        qDebug() << "Cannot allocate audio FIFO";
        goto fail;
    }

    m_audioFrame = av_frame_alloc();
    if (!m_audioFrame) {
        qDebug() << "Cannot allocate audio frame";
        goto fail;
    }

    m_audioFrame->nb_samples = m_audioEnc->frame_size;
    m_audioFrame->format = m_audioEnc->sample_fmt;
    m_audioFrame->ch_layout = m_audioEnc->ch_layout;
    m_audioFrame->sample_rate = m_audioEnc->sample_rate;

    ret = av_frame_get_buffer(m_audioFrame, 0);
    if (ret < 0) {
        qDebug() << "Cannot allocate audio frame buffer:" << ff_err2str(ret);
        goto fail;
    }

    // 创建输出格式上下文
    ret = avformat_alloc_output_context2(&m_formatCtx, nullptr, "ogg", "output.ogg");
    if (ret < 0) {
        qDebug() << "Could not create output context:" << ff_err2str(ret);
        goto fail;
    }

    // 创建音频流
    m_stream = avformat_new_stream(m_formatCtx, m_audioEnc->codec);
    if (!m_stream) {
        qDebug() << "Failed to create output stream";
        goto fail;
    }
    

    // 复制编码器参数到流
    ret = avcodec_parameters_from_context(m_stream->codecpar, m_audioEnc);
    if (ret < 0) {
        qDebug() << "Failed to copy encoder parameters to output stream:" << ff_err2str(ret);
        goto fail;
    }

    // 打开输出文件
    ret = avio_open(&m_formatCtx->pb, "output.ogg", AVIO_FLAG_WRITE);
    if (ret < 0) {
        qDebug() << "Could not open output file:" << ff_err2str(ret);
        goto fail;
    }

    // 写入文件头
    ret = avformat_write_header(m_formatCtx, nullptr);
    if (ret < 0) {
        qDebug() << "Error occurred when writing header:" << ff_err2str(ret);
        goto fail;
    }

    qDebug() << "OPUS encoder initialized: frame_size=" << m_audioEnc->frame_size
	     << "sample_rate=" << m_audioEnc->sample_rate << "channels=" << m_audioEnc->ch_layout.nb_channels;
    m_valid = true;
    return true;

fail:
    if (m_audioFrame) {
        av_frame_free(&m_audioFrame);
        m_audioFrame = nullptr;
    }
    if (m_audioFifo) {
        av_audio_fifo_free(m_audioFifo);
        m_audioFifo = nullptr;
    }
    if (m_audioEnc) {
        avcodec_free_context(&m_audioEnc);
        m_audioEnc = nullptr;
    }
    if (m_formatCtx) {
        if (m_formatCtx->pb) {
            avio_closep(&m_formatCtx->pb);
        }
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
    }
    return false;
}

bool GBSOpusEncoder::addToFifo(const AudioFrame* audioFrame)
{
    if (audioFrame->sample_rate_hz != m_sampleRate ||
        audioFrame->num_channels != m_channels) {
        qDebug() << "Invalid audio frame format: got" << audioFrame->sample_rate_hz << "Hz,"
                 << audioFrame->num_channels << "channels, expected" << m_sampleRate << "Hz,"
                 << m_channels << "channels";
        return false;
    }

    void *data = reinterpret_cast<void *>(const_cast<void *>(audioFrame->data));
    int written = av_audio_fifo_write(m_audioFifo, &data, (int)audioFrame->samples_per_channel * m_channels);
    if (written < audioFrame->samples_per_channel) {
        qDebug() << "Failed to write data to FIFO: wrote" << written << "of"
                 << audioFrame->samples_per_channel << "samples";
        return false;
    }

    return true;
}

void GBSOpusEncoder::encode()
{
    static int64_t next_pts = 0;
    
    AVPacket* pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "Failed to allocate packet";
        return;
    }

    while (av_audio_fifo_size(m_audioFifo) >= m_audioEnc->frame_size) {
        int ret;
        int read = av_audio_fifo_read(m_audioFifo, 
            reinterpret_cast<void**>(m_audioFrame->data), 
            m_audioEnc->frame_size);
            
        if (read != m_audioEnc->frame_size) {
            qDebug() << "Failed to read from FIFO: read" << read << "of"
                     << m_audioEnc->frame_size << "samples";
            av_packet_free(&pkt);
            return;
        }

        // 将音频数据写入文件
        static FILE* fp = fopen("audio_raw.pcm", "wb");
        if (fp) {
            // 直接写入连续的音频数据
            fwrite(m_audioFrame->data[0], sizeof(float), m_audioEnc->frame_size, fp);
            fflush(fp);
        }

        // 只设置一次pts
        m_audioFrame->pts = av_rescale_q(next_pts, 
            AVRational{1, m_audioEnc->sample_rate}, 
            m_audioEnc->time_base);

        next_pts += m_audioFrame->nb_samples;

        ret = avcodec_send_frame(m_audioEnc, m_audioFrame);
        if (ret < 0) {
            qDebug() << "Error sending frame for encoding:" << ff_err2str(ret);
            av_packet_free(&pkt);
            return;
        }

        
        ret = avcodec_receive_packet(m_audioEnc, pkt);
            
	if (ret == 0) {
		sendOut(*pkt);
	}
        
        av_packet_unref(pkt);
        
    }

    av_packet_free(&pkt);
}

void GBSOpusEncoder::sendOut(AVPacket& pkt)
{
    Frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.format = Frame::FRAME_FORMAT_OPUS;
    frame.payload = pkt.data;
    frame.length = pkt.size;
    frame.additionalInfo.audio.nbSamples = m_audioEnc->frame_size;
    frame.additionalInfo.audio.sampleRate = m_audioEnc->sample_rate;
    frame.additionalInfo.audio.channels = m_audioEnc->ch_layout.nb_channels;
    frame.timeStamp = av_rescale_q(pkt.pts, m_audioEnc->time_base, m_stream->time_base);
    // 设置流索引和时间戳
    pkt.pts = av_rescale_q(m_audioFrame->pts, {1, m_sampleRate}, m_audioEnc->time_base);

    qDebug() << "Packet pts:" << pkt.pts << "dts:" << pkt.dts << "size:" << pkt.size
	     << "stream_index:" << pkt.stream_index;
    
    // 写入数据包
    int ret = av_interleaved_write_frame(m_formatCtx, &pkt);
    if (ret < 0) {
        qDebug() << "Error writing packet:" << ff_err2str(ret);
        return;
    }

    //qDebug() << "Delivering OPUS frame: samples=" << frame.additionalInfo.audio.nbSamples
    //         << "sampleRate=" << frame.additionalInfo.audio.sampleRate
    //         << "channels=" << frame.additionalInfo.audio.channels
    //         << "length=" << frame.length;
}

bool GBSOpusEncoder::addAudioFrame(const AudioFrame* audioFrame)
{
    if (!m_valid || !audioFrame) {
        return false;
    }

    if (!addToFifo(audioFrame)) {
        return false;
    }

    encode();
    return true;
}

char* GBSOpusEncoder::ff_err2str(int errRet)
{
    av_strerror(errRet, m_errbuff, sizeof(m_errbuff));
    return m_errbuff;
}

