#ifndef GBSBIZSETTINGPAGE_H
#define GBSBIZSETTINGPAGE_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QSharedPointer>
#include "window-basic-settings.hpp"

namespace Ui {
class GBSBizSettingPage;
}

class GBSBizSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingPage(QWidget *parent = nullptr);
    ~GBSBizSettingPage();

    void markButton(QPushButton *button);

private slots:
    void onClickBasic();
    void onClickAVOutput();
    void onClickAVSetting();

    void onOK();
    void onCancel();
    void onApply();

private:
    Ui::GBSBizSettingPage *ui;
    QWidget *currentWidget{nullptr};

    QList<QPushButton *> buttons;
    QList<QString> iconPathes;
    QSharedPointer<OBSBasicSettings> settings;

    
    

};

#endif // GBSBIZSETTINGPAGE_H
