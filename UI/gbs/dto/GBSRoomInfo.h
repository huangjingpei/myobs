#ifndef __GBS_ROOM_INOF__
#define __GBS_ROOM_INOF__

#include <string>
#include "nlohmann/json.hpp"

class GBSRoomInfo {


public:
	GBSRoomInfo() = default;

	int32_t id;
	int32_t userId;
	int platformType;
	std::string remoteAccount;
	std::string remotePassword;
	std::string equipmentNo;
	std::string sortCode;
	std::string roomId;
	int32_t state;
	int32_t isDel;
	std::string createdTime;
	std::string updatedTime;



	GBSRoomInfo(const int32_t id, const int32_t userId, const int platformType, const std::string remoteAccount, const std::string remotePassword, const std::string equipmentNo, const std::string sortCode, const std::string roomId, const int32_t state, const int32_t isDel, const std::string createdTime, const std::string updatedTime)
		: id(id), userId(userId), platformType(platformType), remoteAccount(remoteAccount), remotePassword(remotePassword), equipmentNo(equipmentNo), sortCode(sortCode), roomId(roomId), state(state), isDel(isDel), createdTime(createdTime), updatedTime(updatedTime)
	{
	}

	void setId(int id);
	int32_t getId();
	void setUserId(int userId);
	int32_t getUserId();
	void setPlatformType(int platformType);
	int32_t getPlatformType();
	void setRemoteAccount(std::string remoteAccount);
	std::string getRemoteAccount();
	void setRemotePassword(std::string remotePassword);
	std::string getRemotePassword();

	void setEquipmentNo(std::string remotePassword);
	std::string getEquipmentNo();

	void setSortCode(std::string sortCode);
	std::string getSortCode();

	void setRoomId(std::string roomId);
	std::string getRoomId();

	void setState(int state);
	int32_t getState();
	void setIsDel(int isDel);
	int32_t getIsDel();

	void setCreateTime(std::string createdTime);
	std::string getCreateTime();
	void setUpdateTime(std::string updatedTime);
	std::string getUpateTime();

	static std::list<GBSRoomInfo> fromJson(const std::string& json);
	static GBSRoomInfo fromJsonByEquipment(const std::string &json);

	nlohmann::json toJson() const;

};


#endif //__GBS_ROOM_INOF__
