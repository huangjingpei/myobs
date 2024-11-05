#ifndef GBSBizDeviceInfo_H
#define GBSBizDeviceInfo_H

#include <QWidget>
#include <QTimer>
#include <util/platform.h>


namespace Ui {
class GBSBizDeviceInfo;
}

class GBSBizDeviceInfo : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizDeviceInfo(QWidget *parent = nullptr);
	~GBSBizDeviceInfo();

public slots:
    void onSliderValueChanged(int value);

    void Update();
    void RecordingTimeLeft();
    static void InitializeValues();


private:
	Ui::GBSBizDeviceInfo *ui;

	QTimer timer;
	QTimer recTimeLeft;
	uint64_t num_bytes = 0;
	os_cpu_usage_info_t *cpu_info = nullptr;
};

#endif // GBSBizDeviceInfo_H
