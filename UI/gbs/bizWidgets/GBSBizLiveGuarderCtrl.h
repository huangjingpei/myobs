#ifndef GBSBIZLIVEGUARDERCTRL_H
#define GBSBIZLIVEGUARDERCTRL_H

#include <QWidget>



namespace Ui {
class GBSBizLiveGuarderCtrl;
}

class GBSBizLiveGuarderCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizLiveGuarderCtrl(QWidget *parent = nullptr);
~GBSBizLiveGuarderCtrl();

public slots:
void onTabChanged(int index);

protected:
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);
	void ResizePreview(uint32_t cx, uint32_t cy);


private:
    Ui::GBSBizLiveGuarderCtrl *ui;

private:
	QWidget *validWidget = nullptr;

	int previewX = 0, previewY = 0;
	int previewCX = 0, previewCY = 0;
	float previewScale = 0.0f;
};

#endif // GBSBIZLIVEGUARDERCTRL_H
