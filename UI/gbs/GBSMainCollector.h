#pragma once
#include <memory>
#include <mutex>
#include <QList>
#include <QString>
#include "gbs/dto/GBSLiveAccountInfo.h"

class GBSMainCollector {
public:
	~GBSMainCollector() = default;
	static std::shared_ptr<GBSMainCollector> getInstance()
	{
		static std::shared_ptr<GBSMainCollector> instance(new GBSMainCollector());
		return instance;
	}

public:
	void setLiveStatus(int status);
	void setLiveDiscardFrames(int frames);
	void setLiveOutputFrames(int frames);
	void setLiveBitrate(int bitrate);
	int getLiveStatus();
	int getLiveDiscardFrames();
	int getLiveOutputFrames();
	int getLiveBitrate();

	void setRecStatus(int status);
	void setRecDiscardFrames(int frames);
	void setRecOutputFrames(int frames);
	void setRecBitrate(int bitrate);
	int getRecStatus();
	int getRecDiscardFrames();
	int getRecOutputFrames();
	int getRecBitrate();

	void reset();
	void resetRecStatus();
	void resetLiveStatus();

	void updateLivePushUrl(std::string url);
	std::string obtainLivePushUrl();

	std::string &getLivePlatAcct();

	void setLiving(bool living);
	bool isLiving();

	std::string& getDeviceName();
	void setDeviceName(std::string plat);


	void setSystemUniqueNo(std::string uniqueNo);
	std::string &getSystemUniqueNo();


private:
	void setSrsLiveId(int srsLiveId);
	void setLiveDeviceId(std::string liveDeviceId);

public:
	std::string& getSrsLiveId();
	std::string &getLiveDeviceId();//直播设备ID，每一个转发设备对应一个不同的ID


	std::string &getDanmaKuName();

	void setDanmakuPlat(std::string plat);
	std::string &getDanmakuPlat();

	//SIGNAL-SHOT 一次性更新
	void setAccountInfo(GBSLiveAccountInfo info);
	GBSLiveAccountInfo& getAccountInfo();

	
	//CONST TYPE
	QList<QString> &getLivePlats();
	QList<QString> &getLiveAbbreviations();

	std::string & getBaseUrl();
	std::string & getBaseUrlV2();
	std::string & getBaseWebSocketV2();


	// const data

	std::string &getSoftWareVersion();
	std::string &getBuildInfo();
	

private:
	GBSMainCollector();

private:
	int mLiveStatus{0};
	int mLiveDiscardFrames{0};
	int mLiveOutputFrames{0};
	int mLiveBitrate{0};

	int mRecStatus{0};
	int mRecDiscardFrames{0};
	int mRecOutputFrames{0};
	int mRecBitrate{0};
	std::string mLivePushUrl{""};
	mutable std::mutex mMutex;

	GBSLiveAccountInfo mAccountInfo;

	QList<QString> livePlats;
	QList<QString> liveAbbreviations;

	std::string danmakuPlat{""};
	std::string srsLiveId {""};
	std::string liveDeviceId{""};//直播设备ID，每一个转发设备对应一个不同的ID
	std::string mPlatLiveAcct{""};
	std::string mDeviceName{""}; //弹幕标签上显示的内容用于区分不同的矩阵
	std::atomic<bool> mbLiving{false};
	std::string mUniqueNo{""};
	std::string danmakuId{""};//主要有srsliveId 和 弹幕被抓平台id构成
	std::string buildInfo{""};
	

	 //std::string baseUrl = {"http://36be34f5.r27.cpolar.top"};
	 //std::string baseUrlV2 = {"http://36be34f5.r27.cpolar.top"};
	 std::string baseUrl = {"https://preferred-api.guobo.shop"};
	 std::string baseUrlV2 = {"https://preferred-api.guobo.shop"};
	 std::string baseWssV2 = {"wss://guobowss.guobo.shop"};

	 //版本构成说明
	 
	 std::string mSoftWareVersion{"1.0.27"};
	
	
};
