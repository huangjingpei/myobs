#pragma once
extern "C" {
	#include <libavutil/opt.h>
	#include <libavutil/mem.h>
	#include <libswresample/swresample.h>
	#include <libavcodec/avcodec.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/audio_fifo.h>
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/frame.h>
	#include <libavutil/error.h>
	#include <libavutil/opt.h>
	#include <libavutil/samplefmt.h>
	#include <libavutil/mem.h>
	#include <libavutil/time.h>
	#include <libavutil/fifo.h>
	#include <libavutil/audio_fifo.h>
}
#include <iostream>
#include <vector>
//resample from int to short.

class GBSAudioResampler2 {
public:
    GBSAudioResampler2(int sample_rate = 16000, int channels = 2, int max_samples = 16000) 
        : sample_rate_(sample_rate),
          channels_(channels),
          max_samples_(max_samples),
          swr_ctx_(nullptr),
          output_data_(nullptr),
          output_linesize_(0),
          output_sample_count_(0) {
    }

    ~GBSAudioResampler2() {
	    std::lock_guard<std::mutex> lock(mutex_);
        if (swr_ctx_) {
            swr_free(&swr_ctx_);
        }
        if (output_data_) {
            av_freep(&output_data_[0]);
            av_freep(&output_data_);
        }
    }

    bool initialize() {
        // 初始化重采样上下文
        swr_ctx_ = swr_alloc();
        if (!swr_ctx_) {
            std::cerr << "Error allocating SwrContext." << std::endl;
            return false;
        }

        // 创建输入和输出通道布局
        AVChannelLayout in_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
        AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;

        // 使用 swr_alloc_set_opts2 设置所有参数
        int ret = swr_alloc_set_opts2(&swr_ctx_,
            &out_ch_layout,                // out_ch_layout
	        AV_SAMPLE_FMT_S16, // out_sample_fmt
            sample_rate_,                  // out_sample_rate
            &in_ch_layout,                 // in_ch_layout
            AV_SAMPLE_FMT_S32,            // in_sample_fmt
            sample_rate_,                  // in_sample_rate
            0,                            // log_offset
            nullptr                       // log_ctx
        );

        if (ret < 0) {
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof(errbuf));
            std::cerr << "Error allocating SwrContext with options: " << errbuf << std::endl;
            return false;
        }


        // 预先分配输出缓冲区
        if (av_samples_alloc_array_and_samples(&output_data_, &output_linesize_, 
            channels_, max_samples_, AV_SAMPLE_FMT_S16, 0) < 0) {
            std::cerr << "Error allocating output samples." << std::endl;
            return false;
        }

	// 初始化重采样器
	ret = swr_init(swr_ctx_);
	if (ret < 0) {
		char errbuf[1024];
		av_strerror(ret, errbuf, sizeof(errbuf));
		std::cerr << "Error initializing the resampling context: " << errbuf << std::endl;
		return false;
	}


        return true;
    }

    bool resample(const std::vector<float>& input_data, int input_sample_count) {
	    std::lock_guard<std::mutex> lock(mutex_);
        if (!swr_ctx_ || !output_data_) {
            std::cerr << "Resampler not properly initialized." << std::endl;
            return false;
        }

        if (input_sample_count > max_samples_) {
            std::cerr << "Input sample count exceeds maximum allowed samples." << std::endl;
            return false;
        }

        AVFrame* input_frame = av_frame_alloc();
        input_frame->format = AV_SAMPLE_FMT_FLT;
        av_channel_layout_default(&input_frame->ch_layout, channels_);
        input_frame->sample_rate = sample_rate_;
        input_frame->nb_samples = input_sample_count;
        //input_frame->data[0] = (uint8_t*)input_data.data();

	int ret = av_frame_get_buffer(input_frame, 0);
	if (ret < 0) {
		char errbuf[1024];
		av_strerror(ret, errbuf, sizeof(errbuf));
		std::cerr << "Error allocating buffer for input_frame: " << errbuf << std::endl;
		av_frame_free(&input_frame);
		return false;
	}

	memcpy(input_frame->data[0], input_data.data(), input_sample_count * sizeof(float));

        ret = swr_convert(swr_ctx_, output_data_, max_samples_,
                            (const uint8_t**)input_frame->data, input_sample_count);
        av_frame_free(&input_frame);

        if (ret < 0) {
            std::cerr << "Error during resampling." << std::endl;
            return false;
        }

        output_sample_count_ = ret;
        return true;
    }

    uint8_t** get_output_data() {
	    std::lock_guard<std::mutex> lock(mutex_);
        return output_data_;
    }


    // 获取重采样后的数据指针（作为 int32_t 数组）
    const int32_t* get_output_samples() const {
	    std::lock_guard<std::mutex> lock(mutex_);
        return reinterpret_cast<const int32_t*>(output_data_[0]);
    }

    // 获取输出样本数
    int get_output_sample_count() const {
	std::lock_guard<std::mutex> lock(mutex_);
        return output_sample_count_;
    }

    // 获取每个样本的字节数
    int get_bytes_per_sample() const {
	    std::lock_guard<std::mutex> lock(mutex_);
        return av_get_bytes_per_sample(AV_SAMPLE_FMT_S32);
    }

    // 获取总字节数
    int get_total_bytes() const {
	    std::lock_guard<std::mutex> lock(mutex_);
        return output_sample_count_ * get_bytes_per_sample();
    }

private:
    int sample_rate_;
    int channels_;
    int max_samples_;    // 最大样本数
    SwrContext* swr_ctx_;
    uint8_t** output_data_;
    int output_linesize_;
    int output_sample_count_;
    mutable std::mutex mutex_;
};
