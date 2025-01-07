#include <obs-module.h>

#include <util/threading.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <sys/stat.h>



#define blog(log_level, format, ...) \
	blog(log_level, "[rtc_source: '%s'] " format, obs_source_get_name(context->source), ##__VA_ARGS__)

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)

#define BUFFER_SIZE_MAX (16000)


struct pcm_source {
    obs_source_t *source;
    
    // 音频配置
    uint32_t sample_rate;
    uint32_t channels;
    
    // 缓冲区
    uint8_t *buffer;
    size_t buffer_size;
    pthread_mutex_t mutex;
};

// 获取源名称
static const char *pcm_source_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return "PCM Audio Source";
}

// 创建源
static void *pcm_source_create(obs_data_t *settings, obs_source_t *source)
{
    struct pcm_source *context = bzalloc(sizeof(struct pcm_source));
    context->source = source;
    
    // 初始化默认参数
    context->sample_rate = 16000;
    context->channels = 2;
    
    // 分配缓冲区
    context->buffer = bzalloc(BUFFER_SIZE_MAX);
    context->buffer_size = 0;
    
    pthread_mutex_init(&context->mutex, NULL);
    
    
    return context;
}

// 销毁源
static void pcm_source_destroy(void *data)
{
    struct pcm_source *context = data;
    
    if (context) {
        pthread_mutex_destroy(&context->mutex);
        bfree(context->buffer);
        bfree(context);
    }
}

// 获取属性
static obs_properties_t *pcm_source_properties(void *data)
{
    UNUSED_PARAMETER(data);
    
    obs_properties_t *props = obs_properties_create();
    
    obs_properties_add_int(props, "sample_rate", "Sample Rate", 
                          16000, 192000, 16000);
    obs_properties_add_int(props, "channels", "Channels", 1, 2, 1);
    
    return props;
}

// 获取默认设置
static void pcm_source_defaults(obs_data_t *settings)
{
    obs_data_set_default_int(settings, "sample_rate", 16000);
    obs_data_set_default_int(settings, "channels", 2);
}

// 更新设置
static void pcm_source_update(void *data, obs_data_t *settings)
{
    struct pcm_source *context = data;
    
    context->sample_rate = (uint32_t)obs_data_get_int(settings, "sample_rate");
    context->channels = (uint32_t)obs_data_get_int(settings, "channels");
}

// 公开的函数，用于输入PCM数据
void push_pcm_data(void *data, const uint8_t *pcm_data, size_t size)
{
	struct pcm_source *context = data;

    if (!context || !pcm_data || size == 0)
        return;
        
    pthread_mutex_lock(&context->mutex);
    
    // 检查缓冲区大小
    if (size > BUFFER_SIZE_MAX) {
        //blog(LOG_WARNING, "PCM data too large, truncating");
        size = BUFFER_SIZE_MAX;
    }
    
    // 复制数据到缓冲区
    memcpy(context->buffer, pcm_data, size);
    context->buffer_size = size;
    
    // 创建音频帧
    struct obs_source_audio audio = {0};
    audio.format = AUDIO_FORMAT_16BIT;
    audio.frames = (uint32_t)(size / (sizeof(short) * context->channels));
    audio.speakers = context->channels == 1 ? SPEAKERS_MONO : SPEAKERS_STEREO;
    audio.samples_per_sec = context->sample_rate;
    audio.timestamp = os_gettime_ns();
    audio.data[0] = context->buffer;


    // 处理编码后的数据
    // 示例：写入文件
    //static FILE *pcm_file = NULL;
    //if (!pcm_file) {
	   // pcm_file = fopen("recv16k.pcm", "wb");
	   // if (!pcm_file) {
		  //  return;
	   // }
    //}
    //fwrite(pcm_data, 1, size, pcm_file);
    //fflush(pcm_file);
    
    // 输出音频
    obs_source_output_audio(context->source, &audio);
    
    pthread_mutex_unlock(&context->mutex);
}

OBS_DECLARE_MODULE()
MODULE_EXPORT const char *obs_module_description(void)
{
	return "rtc audio pcm source ";
}


struct obs_source_info pcm_source = {
    .id = "pcm_audio_source",
    .type = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_AUDIO,
    .get_name = pcm_source_get_name,
    .create = pcm_source_create,
    .destroy = pcm_source_destroy,
    .get_properties = pcm_source_properties,
    .get_defaults = pcm_source_defaults,
    .update = pcm_source_update,
    
};

// 模块加载时注册源
bool obs_module_load(void)
{
    obs_register_source(&pcm_source);
    return true;
}
