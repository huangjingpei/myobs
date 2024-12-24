#include "GBSAACEncoder.h"
#include <stdexcept>

GBSAACEncoder::GBSAACEncoder()
{
    codec_ = avcodec_find_encoder_by_name("aac");
    if (!codec_) {
        throw std::runtime_error("Could not find AAC encoder");
    }
}

GBSAACEncoder::~GBSAACEncoder()
{
    if (frame_) {
        av_frame_free(&frame_);
    }

    if (context_) {
        avcodec_free_context(&context_);
    }

if (samples_) {
        av_freep(&samples_[0]);
        av_freep(&samples_);
    }
}

bool GBSAACEncoder::Initialize(int bitrate, int sampleRate, int channels)
{
    context_ = avcodec_alloc_context3(codec_);
    if (!context_) {
        return false;
    }

    // 设置编码参数
    context_->bit_rate = bitrate * 1000;
    context_->sample_rate = sampleRate;
    av_channel_layout_default(&context_->ch_layout, channels);
    context_->sample_fmt = codec_->sample_fmts[0];
    context_->flags = AV_CODEC_FLAG_GLOBAL_HEADER;

    // 打开编码器
    int ret = avcodec_open2(context_, codec_, nullptr);
    if (ret < 0) {
        return false;
    }

    return InitializeFrame();
}

bool GBSAACEncoder::InitializeFrame()
{
    frame_ = av_frame_alloc();
    if (!frame_) {
        return false;
    }

    frame_size_ = context_->frame_size;
    if (!frame_size_) {
        frame_size_ = 1024; // AAC默认帧大小
    }

    frame_->format = context_->sample_fmt;
    frame_->ch_layout = context_->ch_layout;
    frame_->sample_rate = context_->sample_rate;
    frame_->nb_samples = frame_size_;

    // 计算每个采样的大小
    audio_size_ = av_get_bytes_per_sample(context_->sample_fmt);
    frame_size_bytes_ = frame_size_ * (int)audio_size_;

    // 分配采样缓冲区
    int ret = av_samples_alloc_array_and_samples(
        &samples_,
        nullptr,
        context_->ch_layout.nb_channels,
        frame_size_,
        context_->sample_fmt, 0);

    return ret >= 0;
}

bool GBSAACEncoder::Encode(const uint8_t* const* data, size_t frames,
                         std::vector<uint8_t>& outPacket,
                         int64_t& outPts, int64_t& outDts)
{
    if (frames != frame_size_) {
        return false;
    }

    // 复制输入数据到采样缓冲区
    for (int ch = 0; ch < context_->ch_layout.nb_channels; ch++) {
        memcpy(samples_[ch], data[ch], frame_size_bytes_);
    }

    return DoEncode(outPacket, outPts, outDts);
}

bool GBSAACEncoder::DoEncode(std::vector<uint8_t>& outPacket,
                           int64_t& outPts, int64_t& outDts)
{
    AVRational time_base = {1, context_->sample_rate};
    AVPacket packet = {0};
    
    // 设置帧参数
    frame_->pts = av_rescale_q(total_samples_,
                              {1, context_->sample_rate},
                              context_->time_base);

    // 填充音频帧
    int ret = avcodec_fill_audio_frame(
        frame_,
        context_->ch_layout.nb_channels,
        context_->sample_fmt,
        samples_[0],
        frame_size_bytes_ * context_->ch_layout.nb_channels,
        1);

    if (ret < 0) {
        return false;
    }

    total_samples_ += frame_size_;

    // 发送帧到编码器
    ret = avcodec_send_frame(context_, frame_);
    if (ret < 0) {
        return false;
    }

    // 接收编码后的包
    ret = avcodec_receive_packet(context_, &packet);
    if (ret < 0) {
        return (ret == AVERROR(EAGAIN));
    }

    // 复制编码数据到输出缓冲区
    outPacket.resize(packet.size);
    memcpy(outPacket.data(), packet.data, packet.size);

    // 设置时间戳
    outPts = av_rescale_q(packet.pts, context_->time_base, time_base);
    outDts = av_rescale_q(packet.dts, context_->time_base, time_base);

    av_packet_unref(&packet);
    return true;
}

int GBSAACEncoder::GetSampleRate() const
{
    return context_ ? context_->sample_rate : 0;
}

int GBSAACEncoder::GetChannels() const
{
    return context_ ? context_->ch_layout.nb_channels : 0;
}

AVSampleFormat GBSAACEncoder::GetSampleFormat() const
{
    return context_ ? context_->sample_fmt : AV_SAMPLE_FMT_NONE;
}
