#ifndef GBSLIVEDEVICES_H
#define GBSLIVEDEVICES_H

#include <string>
#include "nlohmann/json.hpp"

class GBSLiveDevices {
public:
    GBSLiveDevices();
    virtual ~GBSLiveDevices();

    // Getters
    std::string getActivationCode() const;
    std::string getCreatedTime() const;
    std::string getDeviceName() const;
    std::string getDeviceNo() const;
    int getId() const;
    int getLiveAccountId() const;
    std::string getLivePlatform() const;
    std::string getNotes() const;
    std::string getPlatformAccount() const;
    std::string getProductNo() const;
    int getRemoteSwitch() const;
    std::string getToDeskAccount() const;
    std::string getToDeskPassword() const;

    // Setters
    void setActivationCode(const std::string& activationCode);
    void setCreatedTime(const std::string& createdTime);
    void setDeviceName(const std::string& deviceName);
    void setDeviceNo(const std::string& deviceNo);
    void setId(int id);
    void setLiveAccountId(int liveAccountId);
    void setLivePlatform(const std::string& livePlatform);
    void setNotes(const std::string& notes);
    void setPlatformAccount(const std::string& platformAccount);
    void setProductNo(const std::string& productNo);
    void setRemoteSwitch(int remoteSwitch);
    void setToDeskAccount(const std::string& toDeskAccount);
    void setToDeskPassword(const std::string& toDeskPassword);


    static GBSLiveDevices fromJson(const std::string& json);
    static std::list<GBSLiveDevices> fromJsonArray(const std::string& json);
	nlohmann::json toJson() const;

    bool operator==(const GBSLiveDevices &other) const noexcept;

private:
    std::string activationCode;
    std::string createdTime;
    std::string deviceName;
    std::string deviceNo;
    int id;
    int liveAccountId;
    std::string livePlatform;
    std::string notes;
    std::string platformAccount;
    std::string productNo;
    int remoteSwitch;
    std::string toDeskAccount;
    std::string toDeskPassword;
};

#endif // GBSLIVEDEVICES_H

