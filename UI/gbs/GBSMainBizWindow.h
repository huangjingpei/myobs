#ifndef GBSMAINBIZWINDOW_H
#define GBSMAINBIZWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QPushButton>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QPoint>
#include "gbs/common/GBSHttpClient.h"
#include "gbs/common/HoriNaviButton.h"

namespace Ui {
class GBSMainBizWindow;
}
using BizUI = Ui::GBSMainBizWindow;
class GBSMainBizWindow : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSMainBizWindow(QWidget *parent = nullptr);
	~GBSMainBizWindow();

protected:
	void keyPressEvent(QKeyEvent *event) override;

	void seeYouNext();

signals:
	void windowClosed();

public slots:
	void onDataClick(bool checked = false);
	void onProductClick(bool checked = false);
	void onLiveClick(bool checked = false);
	void onTranslateClick(bool checked = false);
	void onSettingClick(bool checked = false);
	void onAIClick(bool checked = false);

	void closeWindow();
	void onPopupProfile(bool checked = false);

	
private:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	//void wheelEvent(QWheelEvent *event) override;

private:
	void clearWidgetsFromLayout(QBoxLayout* layout);

public:
    
	void markHoriButton(HoriNaviButton* button);
	BizUI* subMain() { return ui;}

private:
	Ui::GBSMainBizWindow *ui;

	QList<QPushButton*> horiButtons;
	QList<QPushButton*> vertButtons;
	int currentHoriButtonIndex;
	int currentVertButtonIndex;
	QHBoxLayout* naviLayout;
	//QHBoxLayout* bizLayout;
	QSharedPointer<QHBoxLayout> bizLayout;

	QWidget *currentBizWidget;


// 通过 OBSHttpEventHandler 继承
	void onLoginResult(const int result) override;
	void onRtmpPushUrl(const std::string url) override;
	void onPullRtmpUrl(const std::string url) override;
	void onUserInfo(const GBSUserInfo *info) override;
	void onUserFileDownLoad(const std::string &path, int type) override;
	void onRoomInfos(std::list<GBSRoomInfo> &info) override;
	void onRoomInfo(GBSRoomInfo *info) override;
	void onQRcodeInfo(std::string no, std::string url, int status) override;

private:
	bool m_dragging = false;
	QPoint m_dragStartPos;
};

#endif // GBSMAINBIZWINDOW_H
