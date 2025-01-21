#include "GBSAudioReader.h"
#include <QDebug>
#include "obs.h"
#include "obs.hpp"
#include "gbs/common/QBizLogger.h"
//#include "audio-resampler.h"

GBSAudioReader::GBSAudioReader() 
    : m_audioSource(nullptr)
    , m_isOurSource(false)
    , m_isRunning(false){
	m_output_buffer = std::make_unique<unsigned char[]>(1024 * 1024);
	resampler_ = std::make_unique<GBSAudioResampler>();
	resampler_->initialize();
}

GBSAudioReader::~GBSAudioReader() {
    Stop();
}

const char* GBSAudioReader::GetPlatformInputSourceId() const {
#ifdef _WIN32
    return "wasapi_input_capture";
#elif __APPLE__
    return "coreaudio_input_capture";
#else // Linux
    return "pulse_input_capture";
#endif
}

obs_source_t* GBSAudioReader::FindExistingMicSource() {
    obs_source_t* result = nullptr;
    const char* target_id = GetPlatformInputSourceId();
    
    auto enum_sources = [](void* param, obs_source_t* source) {
        auto* data = static_cast<std::pair<obs_source_t**, const char*>*>(param);
        const char* source_id = obs_source_get_id(source);
        if (strcmp(source_id, data->second) == 0) {
            *data->first = source;
            return false; // 停止枚举
        }
        return true;
    };

    std::pair<obs_source_t**, const char*> enum_data(&result, target_id);
    obs_enum_sources(enum_sources, &enum_data);
    return result;
}

obs_source_t* GBSAudioReader::CreateMicSource() {
    const char* source_id = GetPlatformInputSourceId();
    obs_data_t* settings = obs_data_create();
    
    // 设置平台特定的默认设置
#ifdef _WIN32
    // Windows特定设置
    obs_data_set_bool(settings, "use_device_timing", false);
#elif __APPLE__
    // macOS特定设置
#else
    // Linux特定设置
#endif

    obs_source_t* source = obs_source_create(source_id, "Microphone", settings, nullptr);
    obs_data_release(settings);
    return source;
}

void GBSAudioReader::SetupAudioCallback() {
    if (m_audioSource) {
        obs_source_add_audio_capture_callback(m_audioSource, AudioCallback, this);
    }
}

bool GBSAudioReader::Initialize() {
    // 先尝试查找已存在的麦克风源
    m_audioSource = FindExistingMicSource();

    // 从OBS音频源获取音频格式信息
    struct obs_audio_info obs_audio;
    if (obs_get_audio_info(&obs_audio)) {
	    m_audioInfo.samples_per_sec = obs_audio.samples_per_sec;
	    m_audioInfo.speakers = obs_audio.speakers;
	    m_audioInfo.format = AUDIO_FORMAT_FLOAT; // OBS内部使用浮点格式
	    m_audioInfo.channels = get_audio_channels(obs_audio.speakers);
    } else {
	    // 如果获取失败，使用默认值
	    m_audioInfo.samples_per_sec = 44100;
	    m_audioInfo.speakers = SPEAKERS_STEREO;
	    m_audioInfo.format = AUDIO_FORMAT_FLOAT;
	    m_audioInfo.channels = 2;
    }


    if (m_audioSource) {
	    
        // 使用找到的源
        m_isOurSource = false;
        
    } else {
	    QLogE("GBSAudioReader cannot find the microphone source in the system. we need a create new source.");
	    // 如果没有找到，创建新的源
	    m_audioSource = CreateMicSource();
	    if (!m_audioSource) {
		    return false;
	    }
	    m_isOurSource = true;
    }
    
    // 初始化编码器和启动编码线程
    m_isRunning = true;
    InitializeAudioEncoder();
    
    SetupAudioCallback();
    return true;
}

void GBSAudioReader::Stop() {
    m_isRunning = false;
    m_queueCV.notify_one();

    CleanupAudioEncoder();
    
    if (m_audioSource) {
        //obs_source_remove_audio_capture_callback(m_audioSource, AudioCallback, this);
        
        if (m_isOurSource) {
            obs_source_release(m_audioSource);
        }
        
        m_audioSource = nullptr;
    }
}

void GBSAudioReader::InitializeAudioEncoder() {


}

void GBSAudioReader::CleanupAudioEncoder() {

}

void GBSAudioReader::AudioCallback(void* data, obs_source_t* source,
                                   const struct audio_data* audio_dat, bool muted) {
    auto *self = static_cast<GBSAudioReader *>(data);
	if (muted || !audio_dat || !audio_dat->frames) {
        return;
    }

    const size_t frames = audio_dat->frames;
    const size_t channels = self->m_audioInfo.channels;
    // 调整缓冲区大小：直接以 float 样本数计算
    self->m_audioBuffer.resize(frames * channels);

    // 不需要 reinterpret_cast，直接使用 float 指针
    float *buffer_ptr = self->m_audioBuffer.data();
    for (size_t channel = 0; channel < channels; ++channel) {
	    const float *channel_data = reinterpret_cast<const float *>(audio_dat->data[channel]);
        for (size_t frame = 0; frame < frames; ++frame) {
            buffer_ptr[frame * channels + channel] = channel_data[frame];
        }
    }
    self->resampler_->resample(self->m_audioBuffer, (int)(frames * channels));
    //qDebug() << "samples: " << self->resampler_->get_output_samples()
	   //  << " count: " << self->resampler_->get_output_sample_count() << " bytes: " << self->resampler_->get_total_bytes();

    if (self->callback_ != nullptr) {
	    self->callback_->onAudioCapture((void *)self->resampler_->get_output_samples(),
					    (int)(sizeof(short) * self->resampler_->get_output_sample_count()),
					    audio_dat->timestamp);
    }

    //int outputSz = 1024*1024;
    //AudioFrame audioFrame;
    //audioFrame.data = (void *)(self->resampler_->get_output_samples());
    //audioFrame.num_channels = (int)channels;
    //audioFrame.samples_per_channel = 480;
    //audioFrame.sample_rate_hz = 48000;
    //audioFrame.ts = audio_dat->timestamp;
    //self->m_audioEncoder->addAudioFrame(&audioFrame);
    //qDebug() << "audio callback.";
}

void GBSAudioReader::SetAudioReaderCallback(IAudioReaderCallback *callback) {
	callback_ = callback;
}


void GBSAudioReader::OnEncodedCallback(void* param, struct encoder_packet* packet) {
    if (!packet) {
        return;
    }

    // 处理编码后的数据
    // 示例：写入文件
    static FILE* encoded_file = nullptr;
    if (!encoded_file) {
        encoded_file = fopen("encoded_audio.opus", "wb");
        if (!encoded_file) {
            qDebug() << "Failed to create encoded audio file";
            return;
        }
    }

    fwrite(packet->data, 1, packet->size, encoded_file);
    fflush(encoded_file);
}
