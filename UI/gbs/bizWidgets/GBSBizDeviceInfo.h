#ifndef GBSBizDeviceInfo_H
#define GBSBizDeviceInfo_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include <util/platform.h>
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSMemberInfo.h"
#include "gbs/media/GBSAudioReader.h"

#include "obs.h"
#include "ZegoRTCEngine.h"

namespace Ui {
class GBSBizDeviceInfo;
}

class GBSBizDeviceInfo : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSBizDeviceInfo(QWidget *parent = nullptr);
	~GBSBizDeviceInfo();

public slots:
    void onSliderValueChanged(int value);

    void Update();
    void RecordingTimeLeft();
    static void InitializeValues();
	
	qint64 converYMDHMStoSec(std::string &date);
    void copyToClipboard(const QString text);

	void onMemberInfo(GBSMemberInfo info) override;
	void UpdateRecAndLive(obs_output_t *output, bool rec);
public:

	void onAccountInfo(GBSLiveAccountInfo result) override;
	void onActiveResult(int result) override;
	void onUserFileDownLoad(const std::string &path, int type) override;

	QString getRoundedAvator();
	QPixmap getRoundedPixmap(const QPixmap &src, int diameter);

signals:
	void onUseIconUpdate(QString iconPaht);

private:
	Ui::GBSBizDeviceInfo *ui;

	QTimer timer;
	QTimer recTimeLeft;
	uint64_t num_bytes = 0;
	os_cpu_usage_info_t *cpu_info = nullptr;

	long double kbps = 0.0l;
	int first_total = 0;
	int first_dropped = 0;
	uint64_t lastBytesSent = 0;
	uint64_t lastBytesSentTime = 0;
	//std::unique_ptr<GBSMemberInfo> memberInfo;

	std::unique_ptr<GBSAudioReader> audioReader;
	std::shared_ptr<ZegoRTCEngine> rtcEngine;
};

#endif // GBSBizDeviceInfo_H
