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
#include "gbs/dto/GBSMemberInfo.h"



class OBSHttpEventHandler {
public:
	OBSHttpEventHandler() = default;
	virtual ~OBSHttpEventHandler(){}
	virtual void onLoginResult(const int result){};
	virtual void onRtmpPushUrl(const std::string url){};
	virtual void onPullRtmpUrl(const std::string url){};
	virtual void onUserInfo(const GBSUserInfo *info){};
	virtual void onUserFileDownLoad(const std::string &path, int type){};
	virtual void onRoomInfos(std::list<GBSRoomInfo> &info){};
	virtual void onRoomInfo(GBSRoomInfo *info){};
	virtual void onQRcodeInfo(std::string no, std::string url, int status){};
	virtual void onMemberInfo(GBSMemberInfo info){};
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

public:
enum DownLoadType  {
	FILE_AVATOR,
	FILE_QRLOGIN
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

    void createQrCodeScan();
    void createQrCodeScanTask();

    void scanLoginInfo(std::string qrCode);
    void scanLoginInfoTask(std::string qrCode);
    void destroy();

    void downFile(std::string url, std::string path, int type);
    void downFileTask(std::string url, std::string path, int type);

    void memberInfo(std::string userId);
    void memberInfoTask(std::string userId);

    void codeList(int levelId);
    void codeListTask(int levelId);

    void remainingActivation(int levelId);
    void remainingActivationTask(int levelId);

    private:
    std::string getDeviceNo();
	std::string getDeviceNoWithoutBraces();

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
