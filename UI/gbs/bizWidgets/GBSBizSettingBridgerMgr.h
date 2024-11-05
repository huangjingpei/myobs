#ifndef GBSBIZSETTINGBRIDGERMGR_H
#define GBSBIZSETTINGBRIDGERMGR_H

#include <QWidget>

namespace Ui {
class GBSBizSettingBridgerMgr;
}

class GBSBizSettingBridgerMgr : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingBridgerMgr(QWidget *parent = nullptr);
    ~GBSBizSettingBridgerMgr();

private:
    Ui::GBSBizSettingBridgerMgr *ui;
};

#endif // GBSBIZSETTINGBRIDGERMGR_H
