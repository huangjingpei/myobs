#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("udp-sender", "en-US")

#define PORT 12345
#define SERVER_IP "192.168.1.100"  // 替换为目标IP地址

static int sockfd;
static struct sockaddr_in server_addr;

static void udp_send_audio_data(const float *audio_data, size_t frames)
{
    sendto(sockfd, (const char *)audio_data, frames * sizeof(float), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

static void audio_callback(void *param, obs_source_t *source, const struct audio_data *audio_data, bool muted)
{
    if (!muted) {
        udp_send_audio_data((const float *)audio_data->data[0], audio_data->frames);
    }
}

bool obs_module_load(void)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // 初始化UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // 注册音频回调
    obs_source_t *source = obs_get_output_source(0);
    if (source) {
        obs_source_add_audio_capture_callback(source, audio_callback, NULL);
    }

    return true;
}

void obs_module_unload(void)
{
    close(sockfd);
#ifdef _WIN32
    WSACleanup();
#endif
}
