#ifndef __OBS_HTTP_CLIENT_HPP__
#define __OBS_HTTP_CLIENT_HPP__
#include <memory>
#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <list>
#include "gbs/utils/TaskExecutor.h"

#include "gbs/dto/GBSUserInfo.h"
#include "gbs/dto/GBSRoomInfo.h"

class OBSHttpEventHandler {
public:
	OBSHttpEventHandler() = default;
	virtual ~OBSHttpEventHandler(){}
	virtual void onLoginResult(const int result) = 0;
	virtual void onRtmpPushUrl(const std::string url) = 0;
	virtual void onPullRtmpUrl(const std::string url) = 0;
	virtual void onUserInfo(const GBSUserInfo *info) = 0;
	virtual void onUserIconPath(const std::string &path) = 0;
	virtual void onRoomInfos(std::list<GBSRoomInfo> &info) = 0;
	virtual void onRoomInfo(GBSRoomInfo *info) = 0;
};

class GBSHttpClient {

//https://docs.libcpr.org/advanced-usage.html
struct MemFile {
	void *fileBuf;   // file data will be save to
	int64_t readLen; // file bytes
	std::string createTime;
	std::string updateTime;
	std::string endTime;
};

using ApiCallback = std::function<void(int)>;
public:
    virtual ~GBSHttpClient() {}
    static std::shared_ptr<GBSHttpClient> getInstance() {
        static std::shared_ptr<GBSHttpClient> instance(new GBSHttpClient());
        return instance;
    }

    void registerHandler(OBSHttpEventHandler *handler);
    void unRegisterHandler(OBSHttpEventHandler *handler);
    void loginWithCheckVersion(
	    std::string phone, std::string password, int loginType, std::string vendor);

    void addBarrageRoomConfig();

    void getPullStream();

    void checkDeviceNoCreateQrCodeScan();
    void queryByEquipmentNo();
    void queryByEquipmentNoTask();

    void sendWebsocketMsg(std::string msg);
    void sendWebsocketMsgTask(std::string msg);


    void getUserInfo();

    void pageQuery();
    void startLive(std::string &equipments);
    void startLiveTask(std::string &equipments);

    void upRemoteLiveRoomState(std::string ids);
    void upRemoteLiveRoomStateTask(std::string ids);
    void destroy();

private:
    std::string getDeviceNo();

public:

public:
    void loginWithCheckVersionTask(std::string phone, std::string password,
			       int loginType, std::string vendor);

    void addBarrageRoomConfigTask();

    void getPullStreamTask();

    void checkDeviceNoCreateQrCodeScanTask();

    void getUserInfoTask();

    void pageQueryTask();
    
    std::string getEequipmentNo();
    
protected:
    GBSHttpClient(const GBSHttpClient&) = delete;
    GBSHttpClient& operator=(const GBSHttpClient&) = delete;

private:
    GBSHttpClient();

private:
    std::string baseUrl = {"https://preferred-api.guobo.shop"};
	//std::string baseUrl = {"http://75546e84.r27.cpolar.top"};
    std::string httpHost;
    bool verifySsl = {false};
    struct MemFile qrCodeBuf {nullptr, 0};

    GBSUserInfo userInfo;
    GBSRoomInfo equipmentInfo;
    std::list<GBSRoomInfo> roomsInfos;
    std::unique_ptr<TaskExecutor> executor;

    std::mutex cs;


public:
    std::string token;

    std::list<OBSHttpEventHandler *> handlers;


};

/* Functions */


#ifdef __cplusplus
extern "C" {
#endif

void gbsLoginWithCheckVersion(std::string phone, std::string password,
			      int loginType, std::string vendor);

#ifdef __cplusplus
}
#endif

#endif //__OBS_HTTP_CLIENT_HPP__
