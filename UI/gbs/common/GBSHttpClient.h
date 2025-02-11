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
#include "gbs/dto/GBSLiveAccountInfo.h"
#include "gbs/dto/GBSLiveDevices.h"
#include "gbs/dto/GBSPushStreamInfo.h"



class OBSHttpEventHandler {
public:
	OBSHttpEventHandler() = default;
	virtual ~OBSHttpEventHandler(){}
	virtual void onLoginResult(const int result, const std::string token = ""){};
	virtual void onRtmpPushUrl(const std::string url, int liveAccountId) {};
	virtual void onRtmpPushError(std::string errMsg) {};
	virtual void onPullRtmpUrl(const std::string url){};
	virtual void onPushRtmpClosed() {};
	virtual void onUserInfo(const GBSUserInfo *info){};
	virtual void onUserFileDownLoad(const std::string &path, int type){};
	virtual void onRoomInfos(std::list<GBSRoomInfo> &info){};
	virtual void onRoomInfo(GBSRoomInfo *info){};
	virtual void onQRcodeInfo(std::string no, std::string url, int status, const std::string token = ""){};
	virtual void onMemberInfo(GBSMemberInfo info){};
	virtual void onAgreementInfo(std::string richText, int type){};
	virtual void onAccountInfo(GBSLiveAccountInfo result){};
	virtual void onActiveResult(int result){};
	virtual void onHeartBeat(int result){};
	virtual void onEnterGuardCtrl(int result){};
	virtual void onListDevices(std::list<GBSLiveDevices> devices,int pageNum){};
	virtual void onPushStreamInfo(GBSPushStreamInfo info){};
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

    void setRecordedToken(std::string externalToken) { token = externalToken; };

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

    void endLive(std::string id);
    void endLiveTask(std::string id);

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

    void agreement(int type);
    void agreementTask(int type);


    //v2
    void srsScanLoginV2(std::string version);
    void srsScanLoginTaskV2(std::string deviceNo, std::string productNo, std::string version);

    void srsAccountLoginV2(std::string account, std::string password, std::string smsCode, std::string version,
			   int type);
    void srsAccountLoginTaskV2(std::string account, std::string password, std::string smsCode, std::string version,
			       int type);
    void scanLoginQrCodeInfoV2(std::string qrCodeNo);
    void scanLoginQrCodeInfoTaskV2(std::string qrCodeNo);

    void srsLiveAccountInfoV2(std::string version);
    void srsLiveAccountInfoTaskV2(std::string deviceNo, std::string productNo,
				  std::string boarderNo, std::string version);

    void activateDeviceV2(std::string activationCode, std::string deviceName, std::string deviceNo, int liveAccountId,
			  std::string livePlatform, std::string motherboardNo, std::string notes,
			  std::string platformAccount, std::string productNo, std ::string toDeskAccount,
			  std::string toDeskPassword);
    void activateDeviceTaskV2(std::string activationCode, std::string deviceName, std::string deviceNo,
			      int liveAccountId, std::string livePlatform, std::string motherboardNo, std::string notes,
			      std::string platformAccount, std::string productNo, std ::string toDeskAccount,
			      std::string toDeskPassword);

    void createSrsStreamV2(int streamSource);
    void createSrsStreamTaskV2(int streamSource);

    void enterControlV2(std::string password, int liveAccountId);
    void enterControlTaskV2(std::string password, int liveAccountId);

    void pageSrsLiveDeviceV2(int liveAccountId, int pageNum, int pagesize = 30);
    void pageSrsLiveDeviceTaskV2(int liveAccountId, int pageNum, int pagesize = 30);

    void addSrsLiveDeviceV2(std::string activationCode, std::string deviceCode, std::string deviceName,
			    int liveAccountId, std::string livePlatform, std::string notes, std::string platformAccount,
			    std::string toDeskAccount, std::string toDeskPassword);
    void addSrsLiveDeviceTaskV2(std::string activationCode, std::string deviceCode, std::string deviceName,
				int liveAccountId, std::string livePlatform, std::string notes,
				std::string platformAccount, std::string toDeskAccount, std::string toDeskPassword);

    void closeSrsStreamLogV2(int id);
    void closeSrsStreamLogTaskV2(int id);


    void deletedSrsLiveDeviceV2(int id);
    void deletedSrsLiveDeviceTaskV2(int id);

    void modifyZlmLiveDevice(std::string deviceName, int id, std::string livePlatform, std::string notes,
			     std::string platformAccount, int remoteSwitch, std::string toDeskAccount,
			     std::string toDeskPassword);

    void modifyZlmLiveDeviceTaskV2(std::string deviceName, int id, std::string livePlatform, std::string notes,
			     std::string platformAccount, int remoteSwitch, std::string toDeskAccount,
			     std::string toDeskPassword);

    /*
    *	矩阵账号未配置,或矩阵账号已被禁用: 1分钟后刷新接口
	矩阵服务器已停用:1分钟后刷新接口
	当前矩阵设备未激活或矩阵设备已被停用: 1分钟后刷新接口
	矩阵账号未进行直播或直播已结束: 1分钟后刷新接口

	上面这4中 你写个if判断 直接比较返回的字符串 如果符合上面的 就用程序1分钟后刷

	如果返回你rtmp的 你就去拉流
    */
    void getPullStreamUrlV2();
    void getPullStreamUrlTaskV2(std::string deviceNo, std::string productNo, std::string motherboardNo);

    void sendHeartbeatTimeV2(int userId);
    void sendHeartbeatTimeTaskV2(int userId);

    void sendWebsocketMsgV2(std::string msg);
    void sendWebsocketMsgTaskV2(std::string msg);


    // 录播接口
    void queryZlmLiveDevicesV2(int liveAccountId);
    void queryZlmLiveDevicesTaskV2(int liveAccountId);

    void startLiveTranscribeV2(int duration, int streamLogId);
    void startLiveTranscribeTaskV2(int duration, int streamLogId);

    void endLiveTranscribeV2(int streamLogId);
    void endLiveTranscribeTaskV2(int streamLogId);

    void getLiveTranscribeStatusV2(int streamLogId);
    void getLiveTranscribeStatusTaskV2(int streamLogId);


    void queryLiveTranscribeByStreamLogIdV2(int streamLogId);
    void queryLiveTranscribeByStreamLogIdTaskV2(int streamLogId);

    void sendTranscribeLiveHeartBeatV2(int id);
    void sendTranscribeLiveHeartBeatTaskV2(int id);

    void startTranscribeLiveV2(int liveAccountId, std::list<std::string> liveDevices, int streamLogId);
    void startTranscribeLiveTaskV2(int liveAccountId, std::list<std::string> liveDevices, int streamLogId);

    void endTranscribeLiveV2(int id);
    void endTranscribeLiveTaskV2(int id);

    void pageZlmStreamLogV2(int streamLogId, int liveServerId, int pageNum, int pageSize);
    void pageZlmStreamLogTaskV2(int streamLogId, int liveServerId, int pageNum, int pageSize);



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
    
    std::string baseUrl;
    std::string baseUrlV2;
    std::string httpHost;
    std::string httpHostV2;
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


#ifdef __cplusplus
}
#endif

#endif //__OBS_HTTP_CLIENT_HPP__
