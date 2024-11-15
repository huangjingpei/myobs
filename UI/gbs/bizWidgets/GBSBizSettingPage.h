#ifndef GBSBIZSETTINGPAGE_H
#define GBSBIZSETTINGPAGE_H

#include <QWidget>

namespace Ui {
class GBSBizSettingPage;
}

class GBSBizSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingPage(QWidget *parent = nullptr);
    ~GBSBizSettingPage();
private slots:
    void onClickBasic();
    void onClickAVOutput();
    void onClickAVSetting();

private:
    Ui::GBSBizSettingPage *ui;
    QWidget *currentWidget{nullptr};

};

#endif // GBSBIZSETTINGPAGE_H
