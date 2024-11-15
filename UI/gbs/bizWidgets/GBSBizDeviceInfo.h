#ifndef GBSBizDeviceInfo_H
#define GBSBizDeviceInfo_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include <util/platform.h>
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSMemberInfo.h"

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


	void onMemberInfo(GBSMemberInfo info) override;

private:
	Ui::GBSBizDeviceInfo *ui;

	QTimer timer;
	QTimer recTimeLeft;
	uint64_t num_bytes = 0;
	os_cpu_usage_info_t *cpu_info = nullptr;
	//std::unique_ptr<GBSMemberInfo> memberInfo;
};

#endif // GBSBizDeviceInfo_H
