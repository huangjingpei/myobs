
#include <util/platform.h>
#include <obs.h>
#include "./../../../plugins/rtc-source/rtc-source.h"
#include "./../../UI/window-basic-main.hpp"

#include <obs.h>
#include <obs.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "GBSAudioWriter.h"

class GBSAudioWriterImpl : public GBSAudioWriter {
private:
	obs_data_t *settings;
	obs_source_t *pcm_source;
	uint32_t sampleRate;
	uint8_t channels;
	audio_format format;

	// 线程相关成员
	std::thread writer_thread;
	bool running;
	std::queue<std::vector<uint8_t>> data_queue;
	std::mutex queue_mutex;
	std::condition_variable cv;

	// 写入线程函数
	void writerThread();

public:
	GBSAudioWriterImpl();
	virtual ~GBSAudioWriterImpl();

	void write(const uint8_t *data, size_t size) override;
	size_t getQueueSize() override;

	void setSampleRate(uint32_t rate) override;
	void setChannels(uint8_t ch) override;
	void setFormat(audio_format fmt);
};


void GBSAudioWriterImpl::writerThread() {
	while (running) {
		std::vector<uint8_t> data;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			if (!data_queue.empty()) {
				data = std::move(data_queue.front());
				data_queue.pop();
			}
		}
		
		if (!data.empty()) {
			push_pcm_data(obs_obj_get_data((void *)pcm_source),
				      (const uint8_t *)data.data(), data.size());
		}
		
		// 10ms 定时
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
#include "obs.h"

GBSAudioWriterImpl::GBSAudioWriterImpl() : running(true) {
	settings = obs_data_create();
	pcm_source =
		OBSBasic::Get()
			->addPCMAudioSource().Get(); //obs_source_create("pcm_audio_source", "My PCM Source", settings, NULL);

	obs_data_release(settings);
	
	// 启动写入线程
	writer_thread = std::thread(&GBSAudioWriterImpl::writerThread, this);
}

GBSAudioWriterImpl::~GBSAudioWriterImpl() {
	// 停止线程
	running = false;
	cv.notify_one();
	if (writer_thread.joinable()) {
		writer_thread.join();
	}
	
	obs_source_release(pcm_source);
	pcm_source = nullptr;
}

void GBSAudioWriterImpl::write(const uint8_t* data, size_t size) {
	if (!pcm_source || !data || size == 0) {
		return;
	}
	
	
	//// 将数据放入队列
	//{
	//	std::lock_guard<std::mutex> lock(queue_mutex);
	//	data_queue.push(std::vector<uint8_t>(data, data + size));
	//}
	//cv.notify_one();

	if (data != nullptr) {
		push_pcm_data(obs_obj_get_data((void *)pcm_source), data, size);
	}
}

size_t GBSAudioWriterImpl::getQueueSize() {
	std::lock_guard<std::mutex> lock(queue_mutex);
	return data_queue.size();
}

void GBSAudioWriterImpl::setSampleRate(uint32_t rate) { 
	sampleRate = rate; 
}

void GBSAudioWriterImpl::setChannels(uint8_t ch) { 
	channels = ch; 
}

void GBSAudioWriterImpl::setFormat(audio_format fmt) { 
	format = fmt; 
}


std::shared_ptr<GBSAudioWriter> GBSAudioWriter::Create()
{
	return std::make_shared<GBSAudioWriterImpl>();
}
