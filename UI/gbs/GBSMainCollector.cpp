#include "GBSMainCollector.h"
#include <sstream>
#include <iomanip>


GBSMainCollector::GBSMainCollector() {
	livePlats << "抖音"
		  << "快手"
		  << "哔哩哔哩"
		  << "视频号"
		  << "拼多多"
		  << "TikTok"
		  << "Facobook";

	liveAbbreviations << "DY"
			  << "KS"
			  << "BILI"
			  << "SPH"
			  << "PDD"
			  << "TK"
			  << "FB";
}
void GBSMainCollector::setLiveStatus(int status)
{
	const std::lock_guard<std::mutex> lock(mMutex);
	mLiveStatus = status;
}
void GBSMainCollector::setLiveDiscardFrames(int frames) {
	const std::lock_guard<std::mutex> lock(mMutex);
	mLiveDiscardFrames = frames;
}
void GBSMainCollector::setLiveOutputFrames(int frames) {
	const std::lock_guard<std::mutex> lock(mMutex);
	mLiveOutputFrames = frames;
}
void GBSMainCollector::setLiveBitrate(int bitrate) {
	const std::lock_guard<std::mutex> lock(mMutex);
	mLiveBitrate = bitrate;
}
int GBSMainCollector::getLiveStatus() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mLiveStatus;
}
int GBSMainCollector::getLiveDiscardFrames() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mLiveDiscardFrames;
}
int GBSMainCollector::getLiveOutputFrames() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mLiveOutputFrames;
}
int GBSMainCollector::getLiveBitrate() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mLiveBitrate;
}

void GBSMainCollector::setRecStatus(int status)
{
	const std::lock_guard<std::mutex> lock(mMutex);
	mRecStatus = status;
}
void GBSMainCollector::setRecDiscardFrames(int frames)
{
	const std::lock_guard<std::mutex> lock(mMutex);
	mRecDiscardFrames = frames;
}
void GBSMainCollector::setRecOutputFrames(int frames)
{
	const std::lock_guard<std::mutex> lock(mMutex);
	mRecOutputFrames = frames;
}
void GBSMainCollector::setRecBitrate(int bitrate)
{
	const std::lock_guard<std::mutex> lock(mMutex);
	mRecBitrate = bitrate;
}
int GBSMainCollector::getRecStatus()
{
	const std::lock_guard<std::mutex> lock(mMutex);
	return mRecStatus;
}
int GBSMainCollector::getRecDiscardFrames()
{
	const std::lock_guard<std::mutex> lock(mMutex);
	return mRecDiscardFrames;
}
int GBSMainCollector::getRecOutputFrames()
{
	const std::lock_guard<std::mutex> lock(mMutex);
	return mRecOutputFrames;
}
int GBSMainCollector::getRecBitrate()
{
	const std::lock_guard<std::mutex> lock(mMutex);
	return mRecBitrate;
}


void GBSMainCollector::reset() {
	resetRecStatus();
	resetLiveStatus();

}

void GBSMainCollector::resetRecStatus() {
	const std::lock_guard<std::mutex> lock(mMutex);
	mRecStatus = 0;
	mRecDiscardFrames = 0;
	mRecOutputFrames = 0;
	mRecBitrate = 0;
}
void GBSMainCollector::resetLiveStatus() {
	const std::lock_guard<std::mutex> lock(mMutex);
	mLiveStatus = 0;
	mLiveDiscardFrames = 0;
	mLiveOutputFrames = 0;
	mLiveBitrate = 0;
}

void GBSMainCollector::updateLivePushUrl(std::string url) {
	const std::lock_guard<std::mutex> lock(mMutex);
}
std::string GBSMainCollector::obtainLivePushUrl() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return "";
}


void GBSMainCollector::setAccountInfo(GBSLiveAccountInfo info) {
	const std::lock_guard<std::mutex> lock(mMutex);
	mAccountInfo = info;
	setSrsLiveId(info.getId());
	setLiveDeviceId(std::to_string(info.getLiveDeviceId()));
}
GBSLiveAccountInfo& GBSMainCollector::getAccountInfo() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mAccountInfo;
}


QList<QString>& GBSMainCollector::getLivePlats() {

	return livePlats;

}
QList<QString>& GBSMainCollector::getLiveAbbreviations() {

	return liveAbbreviations;
}

std::string & GBSMainCollector::getBaseUrl() {
	return baseUrl;
}
std::string & GBSMainCollector::getBaseUrlV2() {
	return baseUrlV2;
}

std::string &GBSMainCollector::getBaseWebSocketV2()
{
	return baseWssV2;
}


void GBSMainCollector::setSrsLiveId(int liveId) {
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << liveId; // 宽度为2，不足补0
	srsLiveId = ss.str();
}
std::string& GBSMainCollector::getSrsLiveId() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return srsLiveId;
}


void GBSMainCollector::setLiveDeviceId(std::string deviceId){
	liveDeviceId = deviceId;
}

std::string &GBSMainCollector::getLiveDeviceId()
{
	const std::lock_guard<std::mutex> lock(mMutex);
	return liveDeviceId;
}

void GBSMainCollector::setDanmakuPlat(std::string plat)
{
	const std::lock_guard<std::mutex> lock(mMutex);
	danmakuPlat = plat;
}
std::string& GBSMainCollector::getDanmakuPlat() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return danmakuPlat;
}

std::string& GBSMainCollector::getDanmaKuName() {
	std::string srsLiveId = getLiveDeviceId();
	std::string livePlat = getDanmakuPlat();
	if (srsLiveId.empty() || livePlat.empty()) {
		return danmakuId;
	}
	danmakuId = livePlat.substr(0, 1) + srsLiveId;
	return danmakuId;
}

void GBSMainCollector::setLiving(bool living) {
	mbLiving = living;
}
bool GBSMainCollector::isLiving() {
	return mbLiving;
}

std::string& GBSMainCollector::getSoftWareVersion() {return mSoftWareVersion;}

std::string &GBSMainCollector::getLivePlatAcct() {
	const std::lock_guard<std::mutex> lock(mMutex);
	mPlatLiveAcct = mAccountInfo.getPlatformAccount();
	return mPlatLiveAcct;
	
}

void GBSMainCollector::setSystemUniqueNo(std::string uniqueNo)
{
	mUniqueNo = uniqueNo;
}
std::string &GBSMainCollector::getSystemUniqueNo()
{
	assert(!mUniqueNo.empty());
	return mUniqueNo;
}

std::string &GBSMainCollector::getDeviceName() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mDeviceName;
}
void GBSMainCollector::setDeviceName(std::string deviceName) {
	const std::lock_guard<std::mutex> lock(mMutex);
	mDeviceName = deviceName;
}
void GBSMainCollector::setPushStreamInfo(GBSPushStreamInfo info) {
	const std::lock_guard<std::mutex> lock(mMutex);
	mPushStreamInfo = info;
}

GBSPushStreamInfo& GBSMainCollector::getPushStreamInfo() {
	const std::lock_guard<std::mutex> lock(mMutex);
	return mPushStreamInfo;
}

std::string &GBSMainCollector::getBuildInfo()
{
	buildInfo = "";
	buildInfo += BUILD_DATE;
	buildInfo += " ";
	buildInfo += BUILD_TIME;
	
	return buildInfo;
}
