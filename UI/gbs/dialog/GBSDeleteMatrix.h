#ifndef GBSDeleteMatrix_H
#define GBSDeleteMatrix_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>
#include <QMap>

#include "gbs/common/GBSHttpClient.h"
#include "gbs/bizWidgets/tables/liveMngr/LiveMngrWidget.h"

namespace Ui {
class GBSDeleteMatrix;
}

class GBSDeleteMatrix : public QDialog, public OBSHttpEventHandler
{
    Q_OBJECT

public:
    explicit GBSDeleteMatrix(QWidget *parent = nullptr);
    ~GBSDeleteMatrix();

    void setLiveId(int id) { liveId = id; };
    void setLiveWidget(QWidget *widget) { liveWidget = reinterpret_cast<LiveMngrWidget *> (widget); };
    void setRow(int row) { currentRow = row; }

    void onDeletedMatrix(int code) override;

    private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSDeleteMatrix *ui;
	QPoint dragPosition;

	int liveId{-1};
	int currentRow{-1};
	LiveMngrWidget *liveWidget{nullptr};

};

#endif // GBSDeleteMatrix_H
