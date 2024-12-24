#pragma once
#include <obs.h>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "gbs/codec/GBSAudioCodec.h"
#include "gbs/media/GBSAudioResampler.h"

#include "../../libobs/media-io/audio-resampler.h"

// 定义编码数据包结构
struct AudioPacket {
    std::vector<uint8_t> data;
    uint64_t timestamp;
    uint32_t frames;
};
class IAudioReaderCallback {
public:
	virtual ~IAudioReaderCallback() {};
	virtual void onAudioCapture(void *data, int size, uint64_t ts) = 0;
};


class GBSAudioReader {
public:
    GBSAudioReader();
    ~GBSAudioReader();

    // 初始化并尝试获取或创建麦克风源
    bool Initialize();
    // 停止捕获
    void Stop();
    // 获取音频数据回调
    static void AudioCallback(void* data, obs_source_t* source,
                            const struct audio_data* audio_data, bool muted);
    
    void SetAudioReaderCallback(IAudioReaderCallback* callback);

private:
    // 获取平台相关的音频源ID
    const char* GetPlatformInputSourceId() const;
    // 查找已存在的麦克风源
    obs_source_t* FindExistingMicSource();
    // 创建新的麦克风源
    obs_source_t* CreateMicSource();
    // 设置音频回调
    void SetupAudioCallback();

    obs_source_t* m_audioSource;
    std::vector<float> m_audioBuffer;
    bool m_isOurSource;

    audio_resampler_t *resampler;

    // 音频参数
    struct AudioInfo {
        uint32_t samples_per_sec;
        speaker_layout speakers;
        audio_format format;
        uint32_t channels;
    } m_audioInfo;

    // 音频队列相关
    std::queue<AudioPacket> m_audioQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCV;
    bool m_isRunning;

    
    // 新增方法
    void InitializeAudioEncoder();
    void CleanupAudioEncoder();
    static void OnEncodedCallback(void* data, struct encoder_packet* packet);


    private:
    IAudioReaderCallback *callback_;
    std::unique_ptr<unsigned char[]> m_output_buffer;
    std::unique_ptr<GBSAudioResampler> resampler_;
};
