#ifndef GBSMAINWINDOW_H
#define GBSMAINWINDOW_H

#include <QMainWindow>
#include "gbsmainform.h"

namespace Ui {
class GBSMainWindow;
}

class GBSMainWindow : public QMainWindow {
	Q_OBJECT


public:
	explicit GBSMainWindow(QWidget *parent = nullptr);
	~GBSMainWindow();
public slots:
	void slotDestroyLoginWindow();



private:
	Ui::GBSMainWindow *ui;
	GBSMainForm *gbsMainForm;
};

#endif // GBSMAINWINDOW_H
