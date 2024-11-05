#ifndef GBS_HTTPS_SERVER_H
#define GBS_HTTPS_SERVER_H


#include <atomic>
#include <condition_variable>
#include <memory>
#include <string>
#include <list>

#include <iostream>
#include <filesystem>
extern "C" {

#include "mongoose.h"
}


#if defined(__cplusplus)
extern "C" {
#endif



struct GBSHttpsContext {
	int debugLevel;
	std::string httpAddr;
	std::string httpsAddr;
	std::string cert;
	std::string key;
	struct mg_mgr mgr;
	struct mg_connection *c;
	std::atomic<bool> running{false};
	std::condition_variable stopCV;
	std::mutex stopMutex;
	std::shared_ptr<std::thread> serverThread{nullptr};
};

typedef struct GBSHttpsContext GBSHttpsHandle;

void *GBSHttpsInit(std::string httpAddr, std::string httpsAddr,
		   std::string cert = "", std::string key = "");
int GBSHttpsRun(void *handle);
void GBSHttpsDestroy(void *handle);

// const std::list<std::shared_ptr<DammaMSG>>& getDanmuList();

#if defined(__cplusplus)
}
#endif


#endif
