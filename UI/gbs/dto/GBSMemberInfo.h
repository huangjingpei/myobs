#ifndef __GBS_MEMBER_INFO_H__
#define __GBS_MEMBER_INFO_H__
#include <stdio.h>
#include <string>

#include "nlohmann/json.hpp"


//#define PropertyBuilderByName(type, name, access_permission)\
//    access_permission:\
//        type m_##name;\
//    public:\
//    inline void set##name(type v) {\
//        m_##name = v;\
//    }\
//    inline type get##name() {\
//        return m_##name;\
//    }\
//
//#define PointerPropertyBuilderByName(type, name, access_permission)\
//    access_permission:\
//        type* m_##name;\
//    public:\
//        inline void set##name(type* v){\
//            m_##name = v;\
//        }\
//        inline type* get##name(){\
//            return m_##name;\
//        }\

class GBSMemberInfo {
public:
	~GBSMemberInfo(){};

private:
	int codeNum;
	std::string content;
	std::string createdTime;
	int id;
	int langNum;
	double money;
	double originalPrice;
	int seat;
	double seatMoney;
	double sharingRatio;
	int status;
	int time;
	std::string title;
	int type;
	std::string updatedTime;

public:
	int32_t GetCodeNum() const { return codeNum; }

	void SetCodeNum(int32_t codeNum) { codeNum = codeNum; }

	std::string GetContent() const { return content; }

	void SetContent(std::string content) { content = content; }

	std::string GetCreatedTime() const { return createdTime; }

	void SetCreatedTime(std::string createdTime) { createdTime = createdTime; }

	int32_t GetId() const { return id; }

	void SetId(int32_t id) { id = id; }

	int32_t GetLangNum() const { return langNum; }

	void SetLangNum(int32_t langNum) { langNum = langNum; }

	double GetMoney() const { return money; }

	void SetMoney(double money) { money = money; }

	double GetOriginalPrice() const { return originalPrice; }

	void SetOriginalPrice(double originalPrice) { originalPrice = originalPrice; }

	int32_t GetSeat() const { return seat; }

	void SetSeat(int32_t seat) { seat = seat; }

	double GetSeatMoney() const { return seatMoney; }

	void SetSeatMoney(double seatMoney) { seatMoney = seatMoney; }
	
	double GetSharingRatio() const { return sharingRatio; }

	void SetSharingRatio(double sharingRatio) { sharingRatio = sharingRatio; }

	int32_t GetStatus() const { return status; }

	void SetStatus(int32_t status) { status = status; }

	int32_t GetTime() const { return time; }

	void SetTime(int32_t time) { time = time; }

	std::string GetTitle() const { return title; }

	void SetTitle(std::string title) { title = title; }

	int32_t GetType() const { return type; }

	void SetType(int32_t type) { type = type; }

	std::string GetUpdatedTime() const { return updatedTime; }

	void SetUpdatedTime(std::string updatedTime) { updatedTime = updatedTime; }

    static GBSMemberInfo fromJson(const std::string& json);
    nlohmann::json toJson() const;


public:


    GBSMemberInfo(int codeNum, const std::string content, const std::string createdTime, int id, int langNum,
		      double money, double originalPrice, int seat, double seatMoney, double sharingRatio, int status,
		      int time,
		      const std::string &title, int type, const std::string updatedTime)
        : codeNum(codeNum), content(content), createdTime(createdTime), id(id), langNum(langNum), money(money), originalPrice(originalPrice), seat(seat), seatMoney(seatMoney), sharingRatio(sharingRatio), status(status), time(time), title(title), type(type), updatedTime(updatedTime)
    {
    }
    bool operator==(const GBSMemberInfo &other) const;
};
#endif //__GBS_MEMBER_INFO_H__
