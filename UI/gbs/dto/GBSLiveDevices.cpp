#include "GBSLiveDevices.h"

GBSLiveDevices::GBSLiveDevices() {
    id = 0;
    liveAccountId = 0;
    remoteSwitch = 0;
}

GBSLiveDevices::~GBSLiveDevices() {
}

// Getters
std::string GBSLiveDevices::getActivationCode() const { return activationCode; }
std::string GBSLiveDevices::getCreatedTime() const { return createdTime; }
std::string GBSLiveDevices::getDeviceName() const { return deviceName; }
std::string GBSLiveDevices::getDeviceNo() const { return deviceNo; }
int GBSLiveDevices::getId() const { return id; }
int GBSLiveDevices::getLiveAccountId() const { return liveAccountId; }
std::string GBSLiveDevices::getLivePlatform() const { return livePlatform; }
std::string GBSLiveDevices::getNotes() const { return notes; }
std::string GBSLiveDevices::getPlatformAccount() const { return platformAccount; }
std::string GBSLiveDevices::getProductNo() const { return productNo; }
int GBSLiveDevices::getRemoteSwitch() const { return remoteSwitch; }
std::string GBSLiveDevices::getToDeskAccount() const { return toDeskAccount; }
std::string GBSLiveDevices::getToDeskPassword() const { return toDeskPassword; }

// Setters
void GBSLiveDevices::setActivationCode(const std::string& activationCode) { this->activationCode = activationCode; }
void GBSLiveDevices::setCreatedTime(const std::string& createdTime) { this->createdTime = createdTime; }
void GBSLiveDevices::setDeviceName(const std::string& deviceName) { this->deviceName = deviceName; }
void GBSLiveDevices::setDeviceNo(const std::string& deviceNo) { this->deviceNo = deviceNo; }
void GBSLiveDevices::setId(int id) { this->id = id; }
void GBSLiveDevices::setLiveAccountId(int liveAccountId) { this->liveAccountId = liveAccountId; }
void GBSLiveDevices::setLivePlatform(const std::string& livePlatform) { this->livePlatform = livePlatform; }
void GBSLiveDevices::setNotes(const std::string& notes) { this->notes = notes; }
void GBSLiveDevices::setPlatformAccount(const std::string& platformAccount) { this->platformAccount = platformAccount; }
void GBSLiveDevices::setProductNo(const std::string& productNo) { this->productNo = productNo; }
void GBSLiveDevices::setRemoteSwitch(int remoteSwitch) { this->remoteSwitch = remoteSwitch; }
void GBSLiveDevices::setToDeskAccount(const std::string& toDeskAccount) { this->toDeskAccount = toDeskAccount; }
void GBSLiveDevices::setToDeskPassword(const std::string& toDeskPassword) { this->toDeskPassword = toDeskPassword; }

GBSLiveDevices GBSLiveDevices::fromJson(const std::string& jsonStr) {
    GBSLiveDevices device;
    auto json = nlohmann::json::parse(jsonStr);
    
    device.setActivationCode(json["activationCode"].is_null() ? "" : json["activationCode"].get<std::string>());
    device.setCreatedTime(json["createdTime"].is_null() ? "" : json["createdTime"].get<std::string>());
    device.setDeviceName(json["deviceName"].is_null() ? "" : json["deviceName"].get<std::string>());
    device.setDeviceNo(json["deviceNo"].is_null() ? "" : json["deviceNo"].get<std::string>());
    device.setId(json["id"].is_null() ? 0 : json["id"].get<int>());
    device.setLiveAccountId(json["liveAccountId"].is_null() ? 0 : json["liveAccountId"].get<int>());
    device.setLivePlatform(json["livePlatform"].is_null() ? "" : json["livePlatform"].get<std::string>());
    device.setNotes(json["notes"].is_null() ? "" : json["notes"].get<std::string>());
    device.setPlatformAccount(json["platformAccount"].is_null() ? "" : json["platformAccount"].get<std::string>());
    device.setProductNo(json["productNo"].is_null() ? "" : json["productNo"].get<std::string>());
    device.setRemoteSwitch(json["remoteSwitch"].is_null() ? 0 : json["remoteSwitch"].get<int>());
    device.setToDeskAccount(json["toDeskAccount"].is_null() ? "" : json["toDeskAccount"].get<std::string>());
    device.setToDeskPassword(json["toDeskPassword"].is_null() ? "" : json["toDeskPassword"].get<std::string>());
    
    return device;
}

nlohmann::json GBSLiveDevices::toJson() const {
    nlohmann::json json;
    
    json["activationCode"] = activationCode;
    json["createdTime"] = createdTime;
    json["deviceName"] = deviceName;
    json["deviceNo"] = deviceNo;
    json["id"] = id;
    json["liveAccountId"] = liveAccountId;
    json["livePlatform"] = livePlatform;
    json["notes"] = notes;
    json["platformAccount"] = platformAccount;
    json["productNo"] = productNo;
    json["remoteSwitch"] = remoteSwitch;
    json["toDeskAccount"] = toDeskAccount;
    json["toDeskPassword"] = toDeskPassword;
    
    return json;
}

bool GBSLiveDevices::operator==(const GBSLiveDevices &other) const noexcept {
    return activationCode == other.activationCode &&
           createdTime == other.createdTime &&
           deviceName == other.deviceName &&
           deviceNo == other.deviceNo &&
           id == other.id &&
           liveAccountId == other.liveAccountId &&
           livePlatform == other.livePlatform &&
           notes == other.notes &&
           platformAccount == other.platformAccount &&
           productNo == other.productNo &&
           remoteSwitch == other.remoteSwitch &&
           toDeskAccount == other.toDeskAccount &&
           toDeskPassword == other.toDeskPassword;
}

std::list<GBSLiveDevices> GBSLiveDevices::fromJsonArray(const std::string& json) {
    std::list<GBSLiveDevices> devices;
    try {
        nlohmann::json jsonArray = nlohmann::json::parse(json);
        if (!jsonArray.is_array()) {
            throw std::runtime_error("Input JSON is not an array");
        }
        
        for (const auto& item : jsonArray) {
            GBSLiveDevices device;
            device.setActivationCode(item["activationCode"].is_null() ? "" : item["activationCode"].get<std::string>());
            device.setCreatedTime(item["createdTime"].is_null() ? "" : item["createdTime"].get<std::string>());
            device.setDeviceName(item["deviceName"].is_null() ? "" : item["deviceName"].get<std::string>());
            device.setDeviceNo(item["deviceNo"].is_null() ? "" : item["deviceNo"].get<std::string>());
            device.setId(item["id"].is_null() ? 0 : item["id"].get<int>());
            device.setLiveAccountId(item["liveAccountId"].is_null() ? 0 : item["liveAccountId"].get<int>());
            device.setLivePlatform(item["livePlatform"].is_null() ? "" : item["livePlatform"].get<std::string>());
            device.setNotes(item["notes"].is_null() ? "" : item["notes"].get<std::string>());
            device.setPlatformAccount(item["platformAccount"].is_null() ? "" : item["platformAccount"].get<std::string>());
            device.setProductNo(item["productNo"].is_null() ? "" : item["productNo"].get<std::string>());
            device.setRemoteSwitch(item["remoteSwitch"].is_null() ? 0 : item["remoteSwitch"].get<int>());
            device.setToDeskAccount(item["toDeskAccount"].is_null() ? "" : item["toDeskAccount"].get<std::string>());
            device.setToDeskPassword(item["toDeskPassword"].is_null() ? "" : item["toDeskPassword"].get<std::string>());
            
            devices.push_back(device);
        }
    } catch (const std::exception& ) {
        throw;
    }
    return devices;
}
