#ifndef GBSBASICWINDOW_H
#define GBSBASICWINDOW_H

#include <QMainWindow>
#include "common/DanmakuWidget.h"

namespace Ui {
class GBSBasicWindow;
}

class GBSBasicWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit GBSBasicWindow(QWidget *parent = nullptr);
	~GBSBasicWindow();

private:
	Ui::GBSBasicWindow *ui;
	DanmakuWidget *danmaWidget;


};

#endif // GBSBASICWINDOW_H
