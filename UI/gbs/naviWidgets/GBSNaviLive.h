#ifndef GBSNAVILIVE_H
#define GBSNAVILIVE_H

#include <QWidget>

#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>


#include "nlohmann/json.hpp"
#include "gbs/common/WebSocketClient.h"
#include "gbs/bizWidgets/GBSBizLiveBroker.h"

#include <QList>
#include "gbs/common/VertNaviButton.h"

	
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



namespace Ui {
class GBSNaviLive;
}

class GBSNaviLive : public QWidget, public WssEventListener
{
	Q_OBJECT

public:
	explicit GBSNaviLive(QWidget *parent = nullptr);
	~GBSNaviLive();

	void addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget);
signals:
	void singalZBZBClicked();
	void signalDBZBClicked();
	void signalDMSZClicked();
	void signalCKGLClicked();

	void signalDanmakuReceived(const QString &text,
				   const QString &imagePath,
				   const QString &text2,
				   const QString &type);

public slots:
	void onZBZBClicked();
	void onDBZBClicked();
	void onDMSZClicked();
	void onCKGLClicked();

private:
	void mariVertButton(VertNaviButton *button);


private:
	Ui::GBSNaviLive *ui;
	QWeakPointer<QLayout> weakLayoutPtr;
	QWidget* currentWidgetRef;

	GBSBizLiveBroker *gbsBizLiveBroker;
	bool useLiveBroker{false};

	// 通过 WssEventListener 继承
	void onMessage(std::string msg) override;
	void onOpen() override;
	void onClose() override;

	void processDanmaItem(const nlohmann::json jsonObject);

	QList<VertNaviButton *> vertNaviButtons;

	QString danmaPlatIconString;
};

#endif // GBSNAVILIVE_H
