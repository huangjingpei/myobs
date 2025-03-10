#ifndef GBSMSGCONFIRM_H
#define GBSMSGCONFIRM_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>
#include <QMap>

#include "gbs/common/GBSHttpClient.h"
#include "gbs/bizWidgets/tables/liveMngr/LiveMngrWidget.h"

namespace Ui {
class GBSMsgConfirm;
}

class GBSMsgConfirm : public QDialog, public OBSHttpEventHandler
{
    Q_OBJECT

public:
    explicit GBSMsgConfirm(QWidget *parent = nullptr);
    ~GBSMsgConfirm();

    void setLiveId(int id) { liveId = id; };
    void setLiveWidget(QWidget *widget) { liveWidget = reinterpret_cast<LiveMngrWidget *> (widget); };
    void setRow(int row) { currentRow = row; }

    void onDeletedMatrix(int code) override;

    private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSMsgConfirm *ui;
	QPoint dragPosition;

	int liveId{-1};
	int currentRow{-1};
	LiveMngrWidget *liveWidget{nullptr};

};

#endif // GBSMSGCONFIRM_H
