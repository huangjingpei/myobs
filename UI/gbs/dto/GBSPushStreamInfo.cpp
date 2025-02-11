#include "GBSPushStreamInfo.h"

GBSPushStreamInfo::GBSPushStreamInfo() {
    id = 0;
    liveAccountId = 0;
    liveServerId = 0;
    streamSource = 0;
    userId = 0;
}

GBSPushStreamInfo::~GBSPushStreamInfo() {
}

// Getters
int GBSPushStreamInfo::getId() const { return id; }
int GBSPushStreamInfo::getLiveAccountId() const { return liveAccountId; }
int GBSPushStreamInfo::getLiveServerId() const { return liveServerId; }
std::string GBSPushStreamInfo::getPushStreamUrl() const { return pushStreamUrl; }
int GBSPushStreamInfo::getStreamSource() const { return streamSource; }
int GBSPushStreamInfo::getUserId() const { return userId; }

// Setters
void GBSPushStreamInfo::setId(int id) { this->id = id; }
void GBSPushStreamInfo::setLiveAccountId(int liveAccountId) { this->liveAccountId = liveAccountId; }
void GBSPushStreamInfo::setLiveServerId(int liveServerId) { this->liveServerId = liveServerId; }
void GBSPushStreamInfo::setPushStreamUrl(const std::string& pushStreamUrl) { this->pushStreamUrl = pushStreamUrl; }
void GBSPushStreamInfo::setStreamSource(int streamSource) { this->streamSource = streamSource; }
void GBSPushStreamInfo::setUserId(int userId) { this->userId = userId; }

GBSPushStreamInfo GBSPushStreamInfo::fromJson(const std::string& jsonStr) {
    GBSPushStreamInfo device;
    auto json = nlohmann::json::parse(jsonStr);
    
    device.setId(json["id"].is_null() ? 0 : json["id"].get<int>());
    device.setLiveAccountId(json["liveAccountId"].is_null() ? 0 : json["liveAccountId"].get<int>());
    device.setLiveServerId(json["liveServerId"].is_null() ? 0 : json["liveServerId"].get<int>());
    device.setPushStreamUrl(json["pushStreamUrl"].is_null() ? "" : json["pushStreamUrl"].get<std::string>());
    device.setStreamSource(json["streamSource"].is_null() ? 0 : json["streamSource"].get<int>());
    device.setUserId(json["userId"].is_null() ? 0 : json["userId"].get<int>());
    
    return device;
}

nlohmann::json GBSPushStreamInfo::toJson() const {
    nlohmann::json json;
    
    json["id"] = id;
    json["liveAccountId"] = liveAccountId;
    json["liveServerId"] = liveServerId;
    json["pushStreamUrl"] = pushStreamUrl;
    json["streamSource"] = streamSource;
    json["userId"] = userId;
    
    return json;
}

bool GBSPushStreamInfo::operator==(const GBSPushStreamInfo &other) const noexcept {
    return id == other.id &&
           liveAccountId == other.liveAccountId &&
           liveServerId == other.liveServerId &&
           pushStreamUrl == other.pushStreamUrl &&
           streamSource == other.streamSource &&
           userId == other.userId;
}

std::list<GBSPushStreamInfo> GBSPushStreamInfo::fromJsonArray(const std::string& json) {
    std::list<GBSPushStreamInfo> devices;
    try {
        nlohmann::json jsonArray = nlohmann::json::parse(json);
        if (!jsonArray.is_array()) {
            throw std::runtime_error("Input JSON is not an array");
        }
        
        for (const auto& item : jsonArray) {
            GBSPushStreamInfo device;
            device.setId(item["id"].is_null() ? 0 : item["id"].get<int>());
            device.setLiveAccountId(item["liveAccountId"].is_null() ? 0 : item["liveAccountId"].get<int>());
            device.setLiveServerId(item["liveServerId"].is_null() ? 0 : item["liveServerId"].get<int>());
            device.setPushStreamUrl(item["pushStreamUrl"].is_null() ? "" : item["pushStreamUrl"].get<std::string>());
            device.setStreamSource(item["streamSource"].is_null() ? 0 : item["streamSource"].get<int>());
            device.setUserId(item["userId"].is_null() ? 0 : item["userId"].get<int>());
            
            devices.push_back(device);
        }
    } catch (const std::exception& ) {
        throw;
    }
    return devices;
}
