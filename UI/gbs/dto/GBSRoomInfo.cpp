#include "GBSRoomInfo.h"


void GBSRoomInfo::setId(int id) {
	id = id;
}
int32_t GBSRoomInfo::getId() {
	return id;
}
void GBSRoomInfo::setUserId(int userId) {
	userId = userId;
}
int32_t GBSRoomInfo::getUserId() {
	return userId;
}
void GBSRoomInfo::setPlatformType(int platformType) {
	platformType = platformType;
}
int32_t GBSRoomInfo::getPlatformType() {
	return platformType;
}
void GBSRoomInfo::setRemoteAccount(std::string remoteAccount) {
	remoteAccount = remoteAccount;
}
std::string GBSRoomInfo::getRemoteAccount() {
	return remoteAccount;

}
void GBSRoomInfo::setRemotePassword(std::string remotePassword) {
	remotePassword = remotePassword;
}
std::string GBSRoomInfo::getRemotePassword() {
	return remotePassword;

}

void GBSRoomInfo::setEquipmentNo(std::string equipmentNo){
	equipmentNo = equipmentNo;
}
std::string GBSRoomInfo::getEquipmentNo() {
	return equipmentNo;
}

void GBSRoomInfo::setSortCode(std::string sortCode) {
	sortCode = sortCode;
}
std::string GBSRoomInfo::getSortCode() {
	return sortCode;

}

void GBSRoomInfo::setRoomId(std::string roomId) {
	roomId = roomId;
}
std::string GBSRoomInfo::getRoomId() {
	return roomId;

}

void GBSRoomInfo::setState(int state) {
	state = state;
}
int32_t GBSRoomInfo::getState() {
	return state;

}
void GBSRoomInfo::setIsDel(int isDel) {
	isDel = isDel;
}
int32_t GBSRoomInfo::getIsDel() {
	return isDel;
}

void GBSRoomInfo::setCreateTime(std::string createdTime) {
	createdTime = createdTime;
}
std::string GBSRoomInfo::getCreateTime() {
	return createdTime;
}
void GBSRoomInfo::setUpdateTime(std::string updatedTime) {
	updatedTime = updatedTime;
}
std::string GBSRoomInfo::getUpateTime() {
	return updatedTime;
}

std::list<GBSRoomInfo> GBSRoomInfo::fromJson(const std::string& json)
{
	auto jsonObject = nlohmann::json::parse(json);
	std::list<GBSRoomInfo> rooms;
	int32_t count = jsonObject["result"]["total"];
	auto records = jsonObject["result"]["records"];
	for (int i = 0; i < count; i++) {
		auto record = records[i];
		int32_t id = record["id"];
		int32_t userId  = record["userId"];
		int platformType = record["platformType"];
		std::string remoteAccount = record["remoteAccount"];
		std::string remotePassword = record["remotePassword"].is_null() ? "" : record["remotePassword"];
		std::string equipmentNo = record["equipmentNo"];
		std::string sortCode = record["sortCode"];
		std::string roomId = record["roomId"];
		int32_t state = record["state"];
		int32_t isDel = record["isDel"];
		std::string createdTime = record["createdTime"];
		std::string updatedTime = record["updatedTime"];

		GBSRoomInfo info(id, userId, platformType, remoteAccount, remotePassword, equipmentNo, sortCode, roomId, state, isDel, createdTime, updatedTime);
		rooms.push_back(info);
	}
	return rooms;
}

GBSRoomInfo GBSRoomInfo::fromJsonByEquipment(const std::string &json) {
	auto jsonObject = nlohmann::json::parse(json);
	auto record = jsonObject["result"];
	int32_t id = record["id"];
	int32_t userId = record["userId"];
	int platformType = record["platformType"];
	std::string remoteAccount = record["remoteAccount"];
	std::string remotePassword = record["remotePassword"].is_null()
					     ? ""
					     : record["remotePassword"];
	std::string equipmentNo = record["equipmentNo"];
	std::string sortCode = record["sortCode"];
	std::string roomId = record["roomId"];
	int32_t state = record["state"];
	int32_t isDel = record["isDel"];
	std::string createdTime = record["createdTime"];
	std::string updatedTime = record["updatedTime"];
	return GBSRoomInfo(id, userId, platformType, remoteAccount,
				remotePassword, equipmentNo, sortCode, roomId,
				state, isDel, createdTime, updatedTime);

}
nlohmann::json GBSRoomInfo::toJson() const {
	return nlohmann::json::object();
}

