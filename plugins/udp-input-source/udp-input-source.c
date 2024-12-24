#include <obs-module.h>
#include <util/threading.h>
#include <util/platform.h>


#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <unistd.h>

#define PORT 12345
#define BUFFER_SIZE 65536

struct udp_input_source {
	obs_source_t *source;
	int socket_fd;
	pthread_t thread;
	volatile bool active;
	
	// 音频缓冲相关
	uint8_t *audio_buffer;
	size_t buffer_size;
	pthread_mutex_t mutex;
	struct audio_convert_info audio_info;
};

static void *udp_receive_thread(void *data)
{
	struct udp_input_source *context = data;
	uint8_t buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	while (context->active) {
		ssize_t received = recvfrom(context->socket_fd, buffer, BUFFER_SIZE, 0,
									(struct sockaddr *)&client_addr, &addr_len);
		if (received > 0) {
			// 创建音频帧
			struct obs_source_audio audio = {0};
			
			// 设置音频格式
			audio.format = AUDIO_FORMAT_FLOAT;
			audio.samples_per_sec = context->audio_info.samples_per_sec;
			audio.speakers = context->audio_info.speakers;
			
			// 计算帧数：接收到的字节数除以（每个采样的字节数 * 通道数）
			size_t bytes_per_sample = sizeof(float);  // AUDIO_FORMAT_FLOAT 使用 4 字节
			uint32_t num_channels = get_audio_channels(context->audio_info.speakers);
			audio.frames = received / (bytes_per_sample * num_channels);
			
			// 设置音频数据
			audio.data[0] = buffer;
			audio.timestamp = os_gettime_ns();
			
			// 将音频帧发送给 OBS
			obs_source_output_audio(context->source, &audio);
		}
	}

	return NULL;
}

static void udp_input_source_start(struct udp_input_source *context)
{
	context->active = true;
	pthread_create(&context->thread, NULL, udp_receive_thread, context);
}

static void udp_input_source_stop(struct udp_input_source *context)
{
	context->active = false;
	pthread_join(context->thread, NULL);
}

static void udp_input_source_get_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, "samples_per_sec", 44100);
	obs_data_set_default_int(settings, "channels", 2);
}

static obs_properties_t *udp_input_source_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_int(props, "port", "UDP Port", 1024, 65535, 1);
	obs_properties_add_int(props, "samples_per_sec", "Sample Rate", 8000, 192000, 50);
	obs_properties_add_int(props, "channels", "Channels", 1, 8, 1);

	return props;
}

static void udp_input_source_update(void *data, obs_data_t *settings)
{
	struct udp_input_source *context = data;
	
	pthread_mutex_lock(&context->mutex);
	
	// 更新音频配置
	context->audio_info.samples_per_sec = (uint32_t)obs_data_get_int(settings, "samples_per_sec");
	
	// 设置扬声器配置
	int channels = (int)obs_data_get_int(settings, "channels");
	switch (channels) {
		case 1:
			context->audio_info.speakers = SPEAKERS_MONO;
			break;
		case 2:
			context->audio_info.speakers = SPEAKERS_STEREO;
			break;
		// 可以添加更多声道配置
		default:
			context->audio_info.speakers = SPEAKERS_STEREO;
	}
	
	pthread_mutex_unlock(&context->mutex);
}

// 添加 Windows 特定的清理函数
#ifdef _WIN32
static void cleanup_winsock(void)
{
    WSACleanup();
}
#endif

static void *udp_input_source_create(obs_data_t *settings, obs_source_t *source)
{
	struct udp_input_source *context = bzalloc(sizeof(struct udp_input_source));
	context->source = source;

	// 初始化互斥锁
	pthread_mutex_init(&context->mutex, NULL);

	// Windows 需要初始化 WSA
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		blog(LOG_ERROR, "Failed to initialize WinSock");
		bfree(context);
		return NULL;
	}
#endif

	// 创建 socket
	context->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (context->socket_fd < 0) {
		blog(LOG_ERROR, "Failed to create socket");
		bfree(context);
		return NULL;
	}

	// 获取端口号
	uint16_t port = (uint16_t)obs_data_get_int(settings, "port");
	if (port == 0) port = 12345; // 默认端口

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(port)
	};

	if (bind(context->socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		blog(LOG_ERROR, "Failed to bind socket");
		close(context->socket_fd);
		bfree(context);
		return NULL;
	}

	// 更新音频配置
	udp_input_source_update(context, settings);

	// 启动接收线程
	udp_input_source_start(context);
	return context;
}

static void udp_input_source_destroy(void *data)
{
	struct udp_input_source *context = data;
	udp_input_source_stop(context);
	pthread_mutex_destroy(&context->mutex);
	close(context->socket_fd);
	bfree(context);
}

static const char *udp_input_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "UDP Input Source";
}

static struct obs_source_info udp_input_source_info = {
	.id = "udp_input_source",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_AUDIO | OBS_SOURCE_DO_NOT_DUPLICATE | OBS_SOURCE_MONITOR_BY_DEFAULT,
	.get_name = udp_input_source_get_name,
	.create = udp_input_source_create,
	.destroy = udp_input_source_destroy,
	.update = udp_input_source_update,
	.get_defaults = udp_input_source_get_defaults,
	.get_properties = udp_input_source_properties
};

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("udp-output-source", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "UDP Input Source";
}

bool obs_module_load(void)
{
	obs_register_source(&udp_input_source_info);
	return true;
}

void obs_module_unload(void)
{
#ifdef _WIN32
    cleanup_winsock();
#endif
}
