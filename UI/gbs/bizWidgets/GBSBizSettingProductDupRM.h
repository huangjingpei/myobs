#ifndef GBSBIZSETTINGPRODUCTDUPRM_H
#define GBSBIZSETTINGPRODUCTDUPRM_H

#include <QWidget>

namespace Ui {
class GBSBizSettingProductDupRM;
}

class GBSBizSettingProductDupRM : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingProductDupRM(QWidget *parent = nullptr);
    ~GBSBizSettingProductDupRM();

private:
    Ui::GBSBizSettingProductDupRM *ui;
};

#endif // GBSBIZSETTINGPRODUCTDUPRM_H
