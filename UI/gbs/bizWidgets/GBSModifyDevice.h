#ifndef GBS_MODIFY_DEVICE_H
#define GBS_MODIFY_DEVICE_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSModifyDevice;
}

class GBSModifyDevice : public QDialog
{
    Q_OBJECT

public:
    explicit GBSModifyDevice(QWidget *parent = nullptr);
    ~GBSModifyDevice();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public:
    void setLiveDeviceId(int liveDeviceId);

    void setLiveDeviceName(std::string name, bool select =false);
    std::string getLiveDeviceName();

    void setLiveAccountId(std::string id, bool select = false);
    std::string getLiveAccountId();

    void setRemoteAccountId(std::string id, bool select = false);
    std::string getRemoteAccountId();

    void setRemotePassword(std::string id, bool select = false);
    std::string getRemotePassword();

    void setRemarks(std::string remark, std::string remark2, bool select = false);
    std::string getRemarks();

    void setRemoteSwitch(bool enable, bool select = false);
    bool getRemoteSwitch();

    void setLivePlat(std::string plat, int index, bool select = false);
    std::string getLivePlat();

private:
    int liveDeviceId{-1};
    std::string liveDeviceName{""};
    std::string liveAccountId{""};
    std::string remoteAccountId{""};
    std::string remotePassword{""};
    std::string livePlat{""};
    std::string remark{""};
    std::string remark1{""};
    bool remoteSwitch{false};

    private:
    Ui::GBSModifyDevice *ui;
    QPoint dragPosition;


};

#endif // GBS_MODIFY_DEVICE_H
