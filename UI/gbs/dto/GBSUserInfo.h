#ifndef __GBS_USER_INFO_H__
#define __GBS_USER_INFO_H__
#include <stdio.h>
#include <string>

 #include "nlohmann/json.hpp"


class UserAuthResponse {
	std::string authCode; //用户授权验证码
	int32_t authLoginNumber; //授权登录数量
	int32_t uid; //用户id
	 std::string GetAuthCode() const {
	  return authCode;
	 }

	 void SetAuthCode(std::string authCode) {
	  authCode = authCode;
	 }

	 int32_t GetAuthLoginNumber() const {
	  return authLoginNumber;
	 }

	 void SetAuthLoginNumber(int32_t authLoginNumber) {
	  authLoginNumber = authLoginNumber;
	 }

	 int32_t GetUid() const {
	  return uid;
	 }

	 void SetUid(int32_t uid) {
	  uid = uid;
	 }

};

class GBSUserInfo {
public:
	GBSUserInfo() = default;

private:
	int32_t id;
	std::string phone;
	std::string nickname;
	std::string name;
	std::string promoCode;
	std::string head;
	std::string email;
	int32_t sex;
	float rateOfFlow;
	std::string rateOfFlowInfo;
	int32_t usedFlow;
	bool hasAuth;
	float integral;
	double money;
	double cashMoney;
	int32_t memberId; //会员id
	int32_t level; //等级，0：用户，1：VIP1，2：VIP2，3：VIP3
	int32_t levelType; //会员类型，1：个人vip，2：企业vip
	std::string outTime; //vip过期时间
	bool isActing; //是否是代理，1：是
	bool hasBindTk; //是否绑定TK账户，true：已绑定
	std::string accToken; //TK账户
	std::string bindPhone; //绑定手机号
	int empower; //授权 0未授权 1已授权
	int status; //状态，1：启用，2：禁用
	std::string invitingNewPosterUrl; //邀新海报url
	UserAuthResponse userAuthRespone;

public:
	 int32_t GetId() const {
	  return id;
	 }

	 void SetId(int32_t id) {
	  id = id;
	 }

	 std::string GetPhone() const {
	  return phone;
	 }

	 void SetPhone(std::string phone) {
	  phone = phone;
	 }

	 std::string GetNickname() const {
	  return nickname;
	 }

	 void SetNickname(std::string nickname) {
	  nickname = nickname;
	 }

	 std::string GetName() const {
	  return name;
	 }

	 void SetName(std::string name) {
	  name = name;
	 }

	 std::string GetPromoCode() const {
	  return promoCode;
	 }

	 void SetPromoCode(std::string promoCode) {
	  promoCode = promoCode;
	 }

	 std::string GetHead() const {
	  return head;
	 }

	 void SetHead(std::string head) {
	  head = head;
	 }

	 std::string GetEmail() const {
		 return email;
	 }

	 void SetEmail(std::string email) {
		 email = email;
	 }
	 int32_t GetSex() const {
	  return sex;
	 }

	 void SetSex(int32_t sex) {
	  sex = sex;
	 }

	 float GetRateOfFlow() const {
	  return rateOfFlow;
	 }

	 void SetRateOfFlow(float rateOfFlow) {
	  rateOfFlow = rateOfFlow;
	 }

	 std::string GetRateOfFlowInfo() const {
	  return rateOfFlowInfo;
	 }

	 void SetRateOfFlowInfo(std::string rateOfFlowInfo) {
	  rateOfFlowInfo = rateOfFlowInfo;
	 }

	 int32_t GetUsedFlow() const {
	  return usedFlow;
	 }

	 void SetUsedFlow(int32_t usedFlow) {
	  usedFlow = usedFlow;
	 }

	 bool GetHasAuth() const {
	  return hasAuth;
	 }

	 void SetHasAuth(bool hasAuth) {
	  hasAuth = hasAuth;
	 }

	 float GetIntegral() const {
	  return integral;
	 }

	 void SetIntegral(float integral) {
	  integral = integral;
	 }

	 double GetMoney() const {
	  return money;
	 }

	 void SetMoney(double money) {
	  money = money;
	 }

	 double GetCashMoney() const {
	  return cashMoney;
	 }

	 void SetCashMoney(double cashMoney) {
	  cashMoney = cashMoney;
	 }

	 int32_t GetMemberId() const {
	  return memberId;
	 }

	 void SetMemberId(int32_t memberId) {
	  memberId = memberId;
	 }

	 int32_t GetLevel() const {
	  return level;
	 }

	 void SetLevel(int32_t level) {
	  level = level;
	 }

	 int32_t GetLevelType() const {
	  return levelType;
	 }

	 void SetLevelType(int32_t levelType) {
	  levelType = levelType;
	 }

	 std::string GetOutTime() const {
	  return outTime;
	 }

	 void SetOutTime(std::string outTime) {
	  outTime = outTime;
	 }

	 bool GetIsActing() const {
	  return isActing;
	 }

	 void SetIsActing(bool isActing) {
	  isActing = isActing;
	 }

	 bool GetHasBindTk() const {
	  return hasBindTk;
	 }

	 void SetHasBindTk(bool hasBindTk) {
	  hasBindTk = hasBindTk;
	 }

	 std::string GetAccToken() const {
	  return accToken;
	 }

	 void SetAccToken(std::string accToken) {
		accToken = accToken;
	 }

	 std::string GetBindPhone() const {
	  return bindPhone;
	 }

	 void SetBindPhone(std::string bindPhone) {
		 bindPhone = bindPhone;
	 }

	 int GetEmpower() const {
	  return empower;
	 }

	 void SetEmpower(int empower) {
	  empower = empower;
	 }

	 int GetStatus() const {
	  return status;
	 }

	 void SetStatus(int status) {
	  status = status;
	 }

	 std::string GetInvitingNewPosterUrl() const {
	  return invitingNewPosterUrl;
	 }

	 void SetInvitingNewPosterUrl(std::string invitingNewPosterUrl) {
	  invitingNewPosterUrl = invitingNewPosterUrl;
	 }

	 UserAuthResponse GetUserAuthRespone() const {
	  return userAuthRespone;
	 }

	 void SetUserAuthRespone(UserAuthResponse userAuthRespone) {
	  userAuthRespone = userAuthRespone;
	 }

    static GBSUserInfo fromJson(const std::string &json);
	 nlohmann::json toJson() const;


GBSUserInfo(const int32_t &id, const std::string &phone,
		     const std::string &nickname, const std::string &name,
		     const std::string &promoCode, const std::string &head,
		     const std::string &email, const int32_t &sex,
		     float rateOfFlow, const std::string &rateOfFlowInfo,
		     int32_t usedFlow, bool hasAuth, float integral, double money,
		     double cashMoney, const int32_t &memberId,
		     const int32_t &level, const int32_t &levelType,
		     const std::string &outTime, bool isActing, bool hasBindTk,
		     const std::string &accToken, const std::string &bindPhone,
		     int empower, int status,
		     const std::string &invitingNewPosterUrl,
		     const UserAuthResponse &userAuthRespone)
		 : id(id),
		   phone(phone),
		   nickname(nickname),
		   name(name),
		   promoCode(promoCode),
		   head(head),
		   email(email),
		   sex(sex),
		   rateOfFlow(rateOfFlow),
		   rateOfFlowInfo(rateOfFlowInfo),
		   usedFlow(usedFlow),
		   hasAuth(hasAuth),
		   integral(integral),
		   money(money),
		   cashMoney(cashMoney),
		   memberId(memberId),
		   level(level),
		   levelType(levelType),
		   outTime(outTime),
		   isActing(isActing),
		   hasBindTk(hasBindTk),
		   accToken(accToken),
		   bindPhone(bindPhone),
		   empower(empower),
		   status(status),
		   invitingNewPosterUrl(invitingNewPosterUrl),
		   userAuthRespone(userAuthRespone)
	 {
	 }
};
#endif //__GBS_USER_INFO_H__
