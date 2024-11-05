#include "GBSUserInfo.h"

GBSUserInfo GBSUserInfo::fromJson(const std::string &json)
{
	auto jsonObject = nlohmann::json::parse(json);

	int32_t id = jsonObject["result"]["id"];
	std::string phone = jsonObject["result"]["phone"];
	std::string nickname = jsonObject["result"]["nickname"];
	std::string name = jsonObject["result"]["name"];
	std::string promoCode = jsonObject["result"]["promoCode"];
	std::string head = jsonObject["result"]["head"];
	std::string email = jsonObject["result"]["email"];
	int32_t sex = jsonObject["result"]["sex"];
	float rateOfFlow = jsonObject["result"]["rateOfFlow"];
	std::string rateOfFlowInfo =
		jsonObject["result"]["rateOfFlowInfo"].is_null()
			? "null"
			: jsonObject["result"]["rateOfFlowInfo"];
	int32_t usedFlow = jsonObject["result"]["usedFlow"];
	int32_t hasAuth = jsonObject["result"]["hasAuth"];
	float integral = jsonObject["result"]["integral"];
	float money = jsonObject["result"]["money"];
	int32_t cashMoney = jsonObject["result"]["cashMoney"];
	int32_t memberId = jsonObject["result"]["memberId"].is_null() ? 0 : jsonObject["result"]["memberId"].get<int>();

	int32_t level = jsonObject["result"]["level"];
	int32_t levelType = jsonObject["result"]["levelType"].is_null() ? 0 : 1;
	std::string outTime = jsonObject["result"]["outTime"].is_null() ? "23:59:59": jsonObject["result"]["outTime"];
	int32_t isActing = jsonObject["result"]["isActing"];
	int32_t hasBindTk = jsonObject["result"]["hasBindTk"];
	std::string accToken = jsonObject["result"]["accToken"];
	std::string bindPhone = jsonObject["result"]["bindPhone"];
	int empower = jsonObject["result"]["empower"];
	int status = jsonObject["result"]["status"];
	std::string invitingNewPosterUrl =
		jsonObject["result"]["invitingNewPosterUrl"];
	bool isEmpty = jsonObject["result"]["userAuthRespone"].is_null();
	UserAuthResponse userAuthRespone;
	return GBSUserInfo(id, phone, nickname, name, promoCode, head, email,
			   sex, rateOfFlow, rateOfFlowInfo, usedFlow, hasAuth,
			   integral, money, cashMoney, memberId, level,
			   levelType, outTime, isActing, hasBindTk, accToken,
			   bindPhone, empower, status, invitingNewPosterUrl,
			   userAuthRespone);
}
nlohmann::json GBSUserInfo::toJson() const {
	return nlohmann::json::object();
}
