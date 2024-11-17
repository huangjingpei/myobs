#ifndef GBSREMOVEBROKER_H
#define GBSREMOVEBROKER_H

#include <QDialog>

namespace Ui {
class GBSRemoveBroker;
}

class GBSRemoveBroker : public QDialog
{
    Q_OBJECT

public:
    explicit GBSRemoveBroker(QWidget *parent = nullptr);
    ~GBSRemoveBroker();

private:
    Ui::GBSRemoveBroker *ui;
};

#endif // GBSREMOVEBROKER_H
