#ifndef GBSBIZSETTINGTIMBRESQUARE_H
#define GBSBIZSETTINGTIMBRESQUARE_H

#include <QWidget>

namespace Ui {
class GBSBizSettingTimbreSquare;
}

class GBSBizSettingTimbreSquare : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingTimbreSquare(QWidget *parent = nullptr);
    ~GBSBizSettingTimbreSquare();

private:
    Ui::GBSBizSettingTimbreSquare *ui;
};

#endif // GBSBIZSETTINGTIMBRESQUARE_H
