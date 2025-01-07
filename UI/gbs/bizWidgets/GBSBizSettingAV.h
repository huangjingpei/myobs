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
	explicit GBSBizSettingAV(QSharedPointer<OBSBasicSettings> settings, QWidget *parent = nullptr);
	~GBSBizSettingAV();

public:
	void SaveVideoSettings();
	void SaveAudioSettings();

private:
	void LoadResolutionLists();
	void RecalcOutputResPixels(const char *resText);
	void ResetDownscales(uint32_t cx, uint32_t cy, bool ignoreAllSignals = false);
	
	bool WidgetChanged(QWidget *widget);

	void LoadDownscaleFilters();

	bool ValidResolutions();
	void LoadFPSData();
	void LoadFPSFraction(OBSBasic *main);
	void LoadFPSInteger(OBSBasic *main);
	void LoadFPSCommon(OBSBasic *main);


private slots:
	void GeneralChanged();
	void on_cbxBasicResolution_editTextChanged(const QString &text);
	void on_cbxOutputResolution_editTextChanged(const QString &text);

private:
	Ui::GBSBizSettingAV *ui;
	QSharedPointer<OBSBasicSettings> settings;
	uint32_t outputCX = 0;
	uint32_t outputCY = 0;
	bool generalChanged = false;
	bool loading = true;
};

#endif // GBSBIZSETTINGAV_H
