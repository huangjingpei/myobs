#include "GBSLiveTranscribe.h"

GBSLiveTranscribe::GBSLiveTranscribe() {
    id = 0;
    isDel = 0;
    streamLogId = 0;
    transcribeDuration = 0.0;
    transcribeNo = 0;
}

GBSLiveTranscribe::~GBSLiveTranscribe() {
}

// Getters
std::string GBSLiveTranscribe::getCreatedTime() const { return createdTime; }
std::string GBSLiveTranscribe::getFileName() const { return fileName; }
std::string GBSLiveTranscribe::getFileSize() const { return fileSize; }
std::string GBSLiveTranscribe::getFileUrl() const { return fileUrl; }
int GBSLiveTranscribe::getId() const { return id; }
int GBSLiveTranscribe::getIsDel() const { return isDel; }
std::string GBSLiveTranscribe::getLiveRoom() const { return liveRoom; }
std::string GBSLiveTranscribe::getPeriodValidity() const { return periodValidity; }
int GBSLiveTranscribe::getStreamLogId() const { return streamLogId; }
double GBSLiveTranscribe::getTranscribeDuration() const { return transcribeDuration; }
int GBSLiveTranscribe::getTranscribeNo() const { return transcribeNo; }

// Setters
void GBSLiveTranscribe::setCreatedTime(const std::string& createdTime) { this->createdTime = createdTime; }
void GBSLiveTranscribe::setFileName(const std::string& fileName) { this->fileName = fileName; }
void GBSLiveTranscribe::setFileSize(const std::string& fileSize) { this->fileSize = fileSize; }
void GBSLiveTranscribe::setFileUrl(const std::string& fileUrl) { this->fileUrl = fileUrl; }
void GBSLiveTranscribe::setId(int id) { this->id = id; }
void GBSLiveTranscribe::setIsDel(int isDel) { this->isDel = isDel; }
void GBSLiveTranscribe::setLiveRoom(const std::string& liveRoom) { this->liveRoom = liveRoom; }
void GBSLiveTranscribe::setPeriodValidity(const std::string& periodValidity) { this->periodValidity = periodValidity; }
void GBSLiveTranscribe::setStreamLogId(int streamLogId) { this->streamLogId = streamLogId; }
void GBSLiveTranscribe::setTranscribeDuration(double transcribeDuration) { this->transcribeDuration = transcribeDuration; }
void GBSLiveTranscribe::setTranscribeNo(int transcribeNo) { this->transcribeNo = transcribeNo; }

GBSLiveTranscribe GBSLiveTranscribe::fromJson(const std::string& jsonStr) {
    GBSLiveTranscribe device;
    auto json = nlohmann::json::parse(jsonStr);
    
    device.setCreatedTime(json["createdTime"].is_null() ? "" : json["createdTime"].get<std::string>());
    device.setFileName(json["fileName"].is_null() ? "" : json["fileName"].get<std::string>());
    device.setFileSize(json["fileSize"].is_null() ? "" : json["fileSize"].get<std::string>());
    device.setFileUrl(json["fileUrl"].is_null() ? "" : json["fileUrl"].get<std::string>());
    device.setId(json["id"].is_null() ? 0 : json["id"].get<int>());
    device.setIsDel(json["isDel"].is_null() ? 0 : json["isDel"].get<int>());
    device.setLiveRoom(json["liveRoom"].is_null() ? "" : json["liveRoom"].get<std::string>());
    device.setPeriodValidity(json["periodValidity"].is_null() ? "" : json["periodValidity"].get<std::string>());
    device.setStreamLogId(json["streamLogId"].is_null() ? 0 : json["streamLogId"].get<int>());
    device.setTranscribeDuration(json["transcribeDuration"].is_null() ? 0.0 : json["transcribeDuration"].get<double>());
    device.setTranscribeNo(json["transcribeNo"].is_null() ? 0 : json["transcribeNo"].get<int>());
    
    return device;
}

nlohmann::json GBSLiveTranscribe::toJson() const {
    nlohmann::json json;
    
    json["createdTime"] = createdTime;
    json["fileName"] = fileName;
    json["fileSize"] = fileSize;
    json["fileUrl"] = fileUrl;
    json["id"] = id;
    json["isDel"] = isDel;
    json["liveRoom"] = liveRoom;
    json["periodValidity"] = periodValidity;
    json["streamLogId"] = streamLogId;
    json["transcribeDuration"] = transcribeDuration;
    json["transcribeNo"] = transcribeNo;
    
    return json;
}

bool GBSLiveTranscribe::operator==(const GBSLiveTranscribe &other) const noexcept {
    return createdTime == other.createdTime &&
           fileName == other.fileName &&
           fileSize == other.fileSize &&
           fileUrl == other.fileUrl &&
           id == other.id &&
           isDel == other.isDel &&
           liveRoom == other.liveRoom &&
           periodValidity == other.periodValidity &&
           streamLogId == other.streamLogId &&
           transcribeDuration == other.transcribeDuration &&
           transcribeNo == other.transcribeNo;
}

std::list<GBSLiveTranscribe> GBSLiveTranscribe::fromJsonArray(const std::string& json) {
    std::list<GBSLiveTranscribe> devices;
    try {
        nlohmann::json jsonArray = nlohmann::json::parse(json);
        if (!jsonArray.is_array()) {
            throw std::runtime_error("Input JSON is not an array");
        }
        
        for (const auto& item : jsonArray) {
            GBSLiveTranscribe device;
            device.setCreatedTime(item["createdTime"].is_null() ? "" : item["createdTime"].get<std::string>());
            device.setFileName(item["fileName"].is_null() ? "" : item["fileName"].get<std::string>());
            device.setFileSize(item["fileSize"].is_null() ? "" : item["fileSize"].get<std::string>());
            device.setFileUrl(item["fileUrl"].is_null() ? "" : item["fileUrl"].get<std::string>());
            device.setId(item["id"].is_null() ? 0 : item["id"].get<int>());
            device.setIsDel(item["isDel"].is_null() ? 0 : item["isDel"].get<int>());
            device.setLiveRoom(item["liveRoom"].is_null() ? "" : item["liveRoom"].get<std::string>());
            device.setPeriodValidity(item["periodValidity"].is_null() ? "" : item["periodValidity"].get<std::string>());
            device.setStreamLogId(item["streamLogId"].is_null() ? 0 : item["streamLogId"].get<int>());
            device.setTranscribeDuration(item["transcribeDuration"].is_null() ? 0.0 : item["transcribeDuration"].get<double>());
            device.setTranscribeNo(item["transcribeNo"].is_null() ? 0 : item["transcribeNo"].get<int>());
            
            devices.push_back(device);
        }
    } catch (const std::exception& ) {
        throw;
    }
    return devices;
}
