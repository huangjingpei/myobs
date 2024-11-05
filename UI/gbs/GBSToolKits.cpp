#include "GBSToolKits.h"
#include <obs-module.h>
#include <QDebug>
#include "obs.h"
#include "obs.hpp"

#include <iostream>
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}


GBSToolKits::GBSToolKits() {
	//avdevice_register_all(); // 初始化设备

}


void GBSToolKits::list_audio_devices()
{
	const AVInputFormat *input_format = av_find_input_format("dshow"); // Windows 示例
	// Linux 使用 "alsa"
	// macOS 使用 "avfoundation"
	AVDictionary *options = NULL;  
	av_dict_set(&options, "list_devices", "true", 0);  

	if (!input_format) {
		std::cerr << "Could not find input format for audio devices\n";
		return;
	}

	AVDeviceInfoList *device_list = nullptr;
	AVFormatContext *fmt_ctx = nullptr;

	if (avformat_open_input(&fmt_ctx, "video=dummy", input_format, &options) < 0) {
		std::cerr << "Could not open input for audio device listing\n";
		return;
	}

	std::cout << "Available audio devices:\n";
	if (avdevice_list_devices(fmt_ctx, &device_list) >= 0) {
		for (int i = 0; i < device_list->nb_devices; i++) {
			// 遍历设备的媒体类型
			for (int j = 0; j < device_list->devices[i]->nb_media_types; j++) {
				if (device_list->devices[i]->media_types[j] == AVMEDIA_TYPE_AUDIO) {
					std::cout << "Audio Device " << i << ": "
						  << device_list->devices[i]->device_name << "\n";
					qDebug() << "Audio Device: " << device_list->devices[i]->device_name;
					break; // 找到视频类型后可以跳出内层循环
				}
			}
		}
		avdevice_free_list_devices(&device_list);
	} else {
		std::cerr << "Could not list audio devices\n";
	}

	avformat_close_input(&fmt_ctx);
}

void GBSToolKits::list_video_devices()
{
	const AVInputFormat *input_format = av_find_input_format("dshow"); // Windows 示例
	// Linux 使用 "v4l2"
	// macOS 使用 "avfoundation"
	AVDictionary *options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);  

	if (!input_format) {
		std::cerr << "Could not find input format for video devices\n";
		return;
	}

	AVDeviceInfoList *device_list = nullptr;
	AVFormatContext *fmt_ctx = nullptr;

	if (avformat_open_input(&fmt_ctx, "video=dummy", input_format, &options) < 0) {
		std::cerr << "Could not open input for video device listing\n";
		return;
	}

	std::cout << "Available video devices:\n";
	if (avdevice_list_devices(fmt_ctx, &device_list) >= 0) {
		for (int i = 0; i < device_list->nb_devices; i++) {
			// 遍历设备的媒体类型
			for (int j = 0; j < device_list->devices[i]->nb_media_types; j++) {
				if (device_list->devices[i]->media_types[j] == AVMEDIA_TYPE_VIDEO) {
					std::cout << "Video Device " << i << ": "
						  << device_list->devices[i]->device_name << "\n";
					qDebug() << "Video Device: " << device_list->devices[i]->device_name;
					break; // 找到视频类型后可以跳出内层循环
				}
			}
		}
		avdevice_free_list_devices(&device_list);
	} else {
		std::cerr << "Could not list video devices\n";
	}

	avformat_close_input(&fmt_ctx);
}

GBSSource *GBSToolKits::CreateSource(std::string id, std::string name)
{
	return obs_source_create(id.c_str(), name.c_str(), NULL,
				 nullptr);
}

void GBSToolKits::DestroySource(GBSSource *source) {
	if (source != nullptr) {
		obs_source_release(source);
		source = nullptr;
	}
}

std::list<QString> GBSToolKits::EnumDevice(GBSSource *source)
{
	std::list<QString> devices;
	obs_properties_t *ppts = obs_source_properties(source);
	obs_property_t *property = obs_properties_first(ppts);
	while (property) {
		const char *name = obs_property_name(property);
		if (strcmp(name, "video_device_id") == 0) {
			size_t count = obs_property_list_item_count(property);
			for (size_t i = 0; i < count; i++) {
				const char *device_name =
					obs_property_list_item_name(property,
								    i);
				QString qstr = QString::fromUtf8(device_name);
				qDebug() << "EnumAudioDevices Video " << qstr;

				devices.push_back(qstr);
			}
		}
		obs_property_next(&property);
	}
	return devices;
}

std::list<QString> GBSToolKits::EnumAudioDevice(GBSSource *source)
{
	std::list<QString> devices;
	obs_properties_t *ppts = obs_source_properties(source);
	obs_property_t *property = obs_properties_first(ppts);

	while (property) {
		const char *name = obs_property_name(property);
		if (strcmp(name, "audio_device_id") == 0) {
			size_t count = obs_property_list_item_count(property);
			for (size_t i = 0; i < count; i++) {
				const char *device_name =
					obs_property_list_item_name(property, i);
				QString qstr = QString::fromUtf8(device_name);
				qDebug() << "EnumAudioDevices Audio " << qstr;
				devices.push_back(qstr);
			}
		}
		obs_property_next(&property);
	}
	return devices;
}

void GBSToolKits::StartPreview(GBSSource* source, int index){

	obs_properties_t *props = obs_source_properties(source);
	obs_property_t *prop_device =
		obs_properties_get(props, "video_device_id");
	if (prop_device) {
		size_t count = obs_property_list_item_count(prop_device);
		for (size_t i = 0; i < count; ++i) {
			if (i == index) {
				const char *device_name =
					obs_property_list_item_name(prop_device, i);
				const char *device_id =
					obs_property_list_item_string(prop_device, i);

				// 选择合适的设备
				obs_data_t *settings =
					obs_source_get_settings(source);
				obs_data_set_string(settings, "video_device_id",
						    device_id);
				obs_source_update(source, settings);
				obs_data_release(settings);
			}
		}
	}

	//obs_scene_t *scene = obs_scene_create("Scene");
	//obs_sceneitem_t *scene_item = obs_scene_add(scene, source);
	obs_set_output_source(0, source);
}

void GBSToolKits::StopPreview(GBSSource *source)
{

	if (source != nullptr) {
	//   if (!source || !scene || !scene_item)
	//	return;

	//// 1. 从场景中移除摄像头源的场景项
	//obs_sceneitem_remove(scene_item);

	//// 2. 停止任何与摄像头源相关的输出
	obs_set_output_source(0, nullptr);

	//// 3. 断开可能的信号连接（如果有绑定信号槽）
	//// 如果你绑定了信号处理程序，确保断开连接
	//// updatePropertiesSignal.Disconnect();  // 示例信号断开，具体信号处理需根据实际实现

	//// 4. 释放摄像头源
	//obs_source_release(source);

	//// 5. 清理场景对象（如果不再需要场景）
	//obs_scene_release(scene);
}
}

void GBSToolKits::StartRtmpPush(std::string rtmpUrl, GBSSource *source)
{
	// 1. 创建 RTMP 输出
	obs_output_t *output =
		obs_output_create("rtmp_output", "RTMP Output", NULL, nullptr);
	if (!output) {
		// 处理输出创建失败的情况
		return;
	}

	 // 2. 创建 RTMP 服务
	obs_service_t *service = obs_service_create(
		"rtmp_output", "My RTMP Service", nullptr, nullptr);
	if (!service) {
		printf("Failed to create RTMP service.\n");
		obs_output_release(output);
		return;
	}

	// 3. 设置输出配置
	obs_data_t *settings = obs_output_get_settings(output);
	obs_data_set_string(settings, "url", rtmpUrl.c_str());

	// 如果你需要将输入源与输出源关联
	obs_data_set_string(settings, "source", obs_source_get_id(source));
	obs_output_update(output, settings);
	obs_data_release(settings);


	// 设置服务的连接信息
	obs_data_t *settings1 = obs_service_get_settings(service);
	obs_data_set_string(settings1, "server",
			    rtmpUrl.c_str()); // 设置 RTMP 服务器地址
	obs_service_update(service, settings1);
	obs_data_release(settings1);

	// 将服务与输出关联
	obs_output_set_service(output, service);

	// 3. 启动输出
	if (!obs_output_start(output)) {
		// 处理输出启动失败的情况
		obs_output_release(output);
		return;
	}

	// 你可以在这里保存输出对象，如果需要停止流时使用
	// 例如：保存到全局变量或使用智能指针等
}



