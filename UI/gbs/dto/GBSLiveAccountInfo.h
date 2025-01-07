#ifndef __GBS_LIVE_ACCOUNT_INFO_H__
#define __GBS_LIVE_ACCOUNT_INFO_H__
#include <stdio.h>
#include <string>

 #include "nlohmann/json.hpp"



class GBSLiveAccountInfo {
public:
	// 默认构造函数
	GBSLiveAccountInfo() = default;
	
	// 带参数构造函数
	GBSLiveAccountInfo(
		std::string activationCode,
		int activationStatus,
		std::string customerNo,
		std::string deviceCode,
		std::string deviceNo,
		std::string head,
		int id,
		std::string liveAccount,
		int liveDeviceId,
		std::string livePlatform,
		int liveServerId,
		std::string motherboardNo,
		std::string nickname,
		std::string notes,
		std::string platformAccount,
		std::string productNo,
		int remoteSwitch,
		std::string toDeskAccount,
		std::string toDeskPassword,
		int userId,
		std::string version
	);

	// 拷贝赋值运算符
	GBSLiveAccountInfo& operator=(const GBSLiveAccountInfo& other);

	// Getters
	const std::string& getActivationCode() const { return activationCode; }
	int getActivationStatus() const { return activationStatus; }
	const std::string& getCustomerNo() const { return customerNo; }
	const std::string& getDeviceNo() const { return deviceNo; }
	const std::string &getDeviceCode() const { return deviceCode; }
	const std::string& getHead() const { return head; }
	int getId() const { return id; }
	const std::string& getLiveAccount() const { return liveAccount; }
	int getLiveDeviceId() const { return liveDeviceId; }
	const std::string& getLivePlatform() const { return livePlatform; }
	int getLiveServerId() const { return liveServerId; }
	const std::string &getNickname() const { return nickname; }
	const std::string &getMotherboardNo() const { return motherboardNo; }
	const std::string& getNotes() const { return notes; }
	const std::string& getPlatformAccount() const { return platformAccount; }
	const std::string& getProductNo() const { return productNo; }
	int getRemoteSwitch() const { return remoteSwitch; }
	const std::string& getToDeskAccount() const { return toDeskAccount; }
	const std::string& getToDeskPassword() const { return toDeskPassword; }
	int getUserId() const { return userId; }
	const std::string& getVersion() const { return version; }

	// Setters
	void setActivationCode(const std::string& value) { activationCode = value; }
	void setActivationStatus(int value) { activationStatus = value; }
	void setCustomerNo(const std::string& value) { customerNo = value; }
	void setDeviceNo(const std::string& value) { deviceNo = value; }
	void setDeviceCode(const std::string &value) { deviceCode = value; }
	void setHead(const std::string& value) { head = value; }
	void setId(int value) { id = value; }
	void setLiveAccount(const std::string &value) { liveAccount = value; }
	void setLiveDeviceId(int value) { liveDeviceId = value; }
	void setLivePlatform(const std::string& value) { livePlatform = value; }
	void setLiveServerId(int value) { liveServerId = value; }
	void setMotherboardNo(int value) { motherboardNo = value; }
	void setNickname(const std::string& value) { nickname = value; }
	void setNotes(const std::string& value) { notes = value; }
	void setPlatformAccount(const std::string& value) { platformAccount = value; }
	void setProductNo(const std::string& value) { productNo = value; }
	void setRemoteSwitch(int value) { remoteSwitch = value; }
	void setToDeskAccount(const std::string& value) { toDeskAccount = value; }
	void setToDeskPassword(const std::string& value) { toDeskPassword = value; }
	void setUserId(int value) { userId = value; }
	void setVersion(const std::string& value) { version = value; }

	static GBSLiveAccountInfo fromJson(const std::string &json);
	nlohmann::json toJson() const;

	bool operator==(const GBSLiveAccountInfo &other) const noexcept;

private:
	std::string activationCode;
	int activationStatus;
	std::string customerNo;
	std::string deviceCode;
	std::string deviceNo;
	std::string head;
	int id;
	std::string liveAccount;
	int liveDeviceId;
	std::string livePlatform;
	int liveServerId;
	std::string motherboardNo;
	std::string nickname;
	std::string notes;
	std::string platformAccount;
	std::string productNo;
	int remoteSwitch;
	std::string toDeskAccount;
	std::string toDeskPassword;
	int userId;
	std::string version;
	

};
#endif //__GBS_LIVE_ACCOUNT_INFO_H__
