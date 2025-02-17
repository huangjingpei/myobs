#pragma once
#include <QTime>
#include <QString>
enum {
	DANITEM_TYPE_WHOIS = 0,
	DANITEM_TYPE_ALL = 1,
	DANITEM_TYPE_GIFT = 2,
	DANITEM_TYPE_CHAT = 3,
	DANITEM_TYPE_LIKE = 4,
	DANITEM_TYPE_SINGLE = 10,

	

};
typedef struct tagDammaItem {
	QTime  currentTime;
	QString deviceName;
	QString iamgePath;
	QString danmaku;
	QString type;
	QString liveId;
} DanmaItem;


	
class DammaMSG {
public:
	virtual ~DammaMSG() = default;
	std::string type;
	std::string name;
	std::string head_image;
	std::string content;
	int msgType{1};
};

//1.	进入房间
class DammaMemberMSG : public DammaMSG {
public:
	virtual ~DammaMemberMSG() override = default;
	int msgType{1};
};

//2. 用户发言
class DanmaChatMessage : public DammaMSG {
public:
	virtual ~DanmaChatMessage() override = default;
	int msgType{2};
};

//3. 用户送礼
class DanmaGiftMessage : public DammaMSG {
public:
	virtual ~DanmaGiftMessage() override = default;
	std::string gift_name;
	std::string gift_count;
	int msgType{3};
};

//4.	用户关注
class DanmaSocialMessage : public DammaMSG {
public:
	virtual ~DanmaSocialMessage() override = default;
	int msgType{4};
};

//5. 用户点赞
class DanmaLikeMessage : public DammaMSG {
public:
	virtual ~DanmaLikeMessage() override = default;
	std::string count;
	int msgType{5};
};

//6. 直播间人数
class DanmaRoomMessage : public DammaMSG {
public:
	virtual ~DanmaRoomMessage() override = default;
	std::string count;
};

//7. 系统消息
class DanmaSystemMessage : public DammaMSG {
public:
	virtual ~DanmaSystemMessage() override = default;
};

class IDanmakuDispatcher {
public:
	IDanmakuDispatcher() = default;
	virtual ~IDanmakuDispatcher() {}
	virtual void onRecvMemberMessage() = 0;
	virtual void onRecvChatMessage() = 0;
	virtual void onRecvGiftMessage() = 0;
	virtual void onRecvSocialMessage() = 0;
	virtual void onRecvLikeMessage() = 0;
};

