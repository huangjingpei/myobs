#ifndef GBSBIZLIVEGUARDERCTRL_H
#define GBSBIZLIVEGUARDERCTRL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSLiveDevices.h"
#include "gbs/common/WebSocketClient.h"
#include "gbs/common/DanmakuWidget.h"

#include "gbs/GBSDanmaType.h"
namespace Ui {
class GBSBizLiveGuarderCtrl;
}
class GridButtons;
class LiveManageWidget;
class GBSBizLiveGuarderCtrl : public QWidget, public OBSHttpEventHandler, public WssEventListener {
    Q_OBJECT

public:
	explicit GBSBizLiveGuarderCtrl(QWidget *parent = nullptr);
	~GBSBizLiveGuarderCtrl();

	void onListDevices(std::list<GBSLiveDevices> devices, int pageNum) override;

signals:

	void signalDanmakuReceived(const QString &text, const QString &imagePath, const QString &text2,
				   const QString &type);

public slots:
	void onTabChanged(int index);
	void addNewWidget(const QString &atext, const QString &aimagePath, const QString &atext2,const QString &atype);

protected:
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);
	void ResizePreview(uint32_t cx, uint32_t cy);

	void processDanmaItem(const nlohmann::json jsonObject);

private:
	// 通过 WssEventListener 继承
	void onMessage(std::string msg) override;
	void onOpen() override;
	void onClose() override;


private:
    Ui::GBSBizLiveGuarderCtrl *ui;

private:
	QWidget *validWidget = nullptr;

	int previewX = 0, previewY = 0;
	int previewCX = 0, previewCY = 0;
	float previewScale = 0.0f;
	GridButtons *gridButtons;
	LiveManageWidget *liveManageWidget;
	std::list<GBSLiveDevices> currentliveDevices;
	int currentPageNum;
	QString danmaPlatIconString;
	std::shared_ptr<WebSocketClient> mWebSocketClient;

	
	QList<DanmaItem> whoIsDanmukus;
	QList<DanmaItem> allDanmakus;
	QList<DanmaItem> giftDanmakus;
	QList<DanmaItem> likeDanmakus;

	QVBoxLayout *danmaKuAreaLayout;
	QScrollArea *danmakuscrollArea;
	QList<DanmakuWidget *> widgetList;
};

#endif // GBSBIZLIVEGUARDERCTRL_H
