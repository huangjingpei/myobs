#pragma once
#include <string>
#include <memory>
#include <list>
#include <mutex>
class WssEventListener {
public:
	virtual ~WssEventListener(){};
	virtual void onMessage(std::string msg) = 0;
	virtual void onOpen() = 0;
	virtual void onClose() = 0;
};

class WebSocketClient {
public:
	static std::shared_ptr<WebSocketClient> getInstance();

	WebSocketClient();
	virtual ~WebSocketClient();
	//static std::shared_ptr<WebSocketClient> Create();
	virtual void Start(std::string url) = 0;
	virtual void Stop() = 0;
	virtual bool IsRunnig() = 0;
	virtual void SetReconnect(bool reconnect = true) = 0;
	virtual void Send(std::string msg) = 0;

	void RegisterHandler(WssEventListener *handler);
	void UnRegisterHandler(WssEventListener *handler);

protected:
	//WssEventListener *listener_;
	std::mutex listenerMutex;
	std::list<WssEventListener *> listeners;
};
