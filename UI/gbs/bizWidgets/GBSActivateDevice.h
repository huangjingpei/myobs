#ifndef __GBS_ADD_CONSUMER_H__
#define __GBS_ADD_CONSUMER_H__
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSActivateDevice;
}

class GBSActivateDevice : public QDialog
{
    Q_OBJECT

public:
    explicit GBSActivateDevice(QWidget *parent = nullptr);
    ~GBSActivateDevice();

    void addCustomerNo(std::string customNo);
    void addLiveAccountId(int liveAccountId);
    void addActivateCode(std::string activateCode);
    void addPlatformAccount(std::string platformAccount);
    void addRemark1(std::string mark);
    void addRemark2(std::string mark);
    void addRemoteUsername(std::string username);
    void addRemotePassword(std::string password);
    void addRemoteSwitcher(int value);

    void diableActivate();

    private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void copyToClipboard(const QString text);

private:
    Ui::GBSActivateDevice *ui;
	QPoint dragPosition;


};

#endif // __GBS_ADD_CONSUMER_H__
