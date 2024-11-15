#include "GBSMemberInfo.h"

GBSMemberInfo GBSMemberInfo::fromJson(const std::string &json)
{
	auto jsonObject = nlohmann::json::parse(json);
	auto result = jsonObject["result"];
	
	int codeNum = result["codeNum"].get<int>();
	std::string content = result["content"].get<std::string>();
	std::string createdTime = result["createdTime"].get<std::string>();
	int id = result["id"].get<int>();
	int langNum = result["langNum"].get<int>();
	double money = result["money"].get<double>();
	double originalPrice = result["originalPrice"].get<double>();
	int seat = result["seat"].get<int>();
	double seatMoney = result["seatMoney"].get<double>();
	double sharingRatio = result["sharingRatio"].get<double>();
	int status = result["status"].get<int>();
	int time = result["time"].get<int>();
	std::string title = result["title"].get<std::string>();
	int type = result["type"].get<int>();
	std::string updatedTime = result["updatedTime"].get<std::string>();




	return GBSMemberInfo(codeNum, content, createdTime, id, langNum, money, originalPrice, seat, seatMoney,
			     sharingRatio, status, time, title, type, updatedTime);
	
	

}
nlohmann::json GBSMemberInfo::toJson() const {
	return nlohmann::json::object();
}

bool GBSMemberInfo::operator==(const GBSMemberInfo &other) const
{
	return false;
}
