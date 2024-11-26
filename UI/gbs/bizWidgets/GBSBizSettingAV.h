#ifndef GBSBIZSETTINGAV_H
#define GBSBIZSETTINGAV_H

#include <QWidget>
#include "window-basic-settings.hpp"
namespace Ui {
class GBSBizSettingAV;
}

class GBSBizSettingAV : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingAV(OBSBasicSettings* settings, QWidget *parent = nullptr);
	~GBSBizSettingAV();

public:
	void SaveVideoSettings();
	void SaveAudioSettings();

private:
	void LoadResolutionLists();
	void RecalcOutputResPixels(const char *resText);
	void ResetDownscales(uint32_t cx, uint32_t cy, bool ignoreAllSignals);
	
	bool WidgetChanged(QWidget *widget);

private slots:
	void GeneralChanged();

private:
	Ui::GBSBizSettingAV *ui;
	OBSBasicSettings *settings;
	uint32_t outputCX = 0;
	uint32_t outputCY = 0;
	bool generalChanged = false;
	bool loading = true;
};

#endif // GBSBIZSETTINGAV_H
