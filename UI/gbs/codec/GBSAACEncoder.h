#pragma once

#include <obs.hpp>
#include <memory>
#include <vector>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
}

class GBSAACEncoder {
public:
    GBSAACEncoder();
    ~GBSAACEncoder();

    // 初始化编码器
    bool Initialize(int bitrate, int sampleRate, int channels);
    
    // 编码音频数据
    bool Encode(const uint8_t* const* data, size_t frames, 
               std::vector<uint8_t>& outPacket,
               int64_t& outPts, int64_t& outDts);

    // 获取编码器参数
    int GetFrameSize() const { return frame_size_; }
    int GetSampleRate() const;
    int GetChannels() const;
    AVSampleFormat GetSampleFormat() const;

private:
    // 初始化音频帧
    bool InitializeFrame();
    
    // 实际的编码过程
    bool DoEncode(std::vector<uint8_t>& outPacket, 
                 int64_t& outPts, int64_t& outDts);

private:
    const AVCodec* codec_ = nullptr;
    AVCodecContext* context_ = nullptr;
    AVFrame* frame_ = nullptr;
    
    uint8_t** samples_{nullptr};
    int64_t total_samples_ = 0;
    
    int frame_size_ = 0;
    int frame_size_bytes_ = 0;
    size_t audio_size_ = 0;
};