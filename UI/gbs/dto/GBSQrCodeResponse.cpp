#include "GBSQrCodeResponse.h"


GBSQrCodeResponse GBSQrCodeResponse::fromJson(const std::string& json) {
    try {
        nlohmann::json j = nlohmann::json::parse(json);
        
        std::string account = j["account"].is_null() ? "" : j["account"].get<std::string>();
        std::string createdTime = j["createdTime"].is_null() ? "" : j["createdTime"].get<std::string>();
        std::string endTime = j["endTime"].is_null() ? "" : j["endTime"].get<std::string>();
        int id = j["id"].is_null() ? 0 : j["id"].get<int>();
        std::string qrCodeNo = j["qrCodeNo"].is_null() ? "" : j["qrCodeNo"].get<std::string>();
        std::string qrCodeUrl = j["qrCodeUrl"].is_null() ? "" : j["qrCodeUrl"].get<std::string>();
        int status = j["status"].is_null() ? 0 : j["status"].get<int>();
        std::string token = j["token"].is_null() ? "" : j["token"].get<std::string>();
        std::string updatedTime = j["updatedTime"].is_null() ? "" : j["updatedTime"].get<std::string>();
        int userId = j["userId"].is_null() ? 0 : j["userId"].get<int>();
        
        GBSQrCodeResponse response;
        response.setAccount(account);
        response.setCreatedTime(createdTime);
        response.setEndTime(endTime);
        response.setId(id);
        response.setQrCodeNo(qrCodeNo);
        response.setQrCodeUrl(qrCodeUrl);
        response.setStatus(status);
        response.setToken(token);
        response.setUpdatedTime(updatedTime);
        response.setUserId(userId);
        
        return response;
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
    }
}

nlohmann::json GBSQrCodeResponse::toJson() const {
    nlohmann::json j;
    j["account"] = account;
    j["createdTime"] = createdTime; 
    j["endTime"] = endTime;
    j["id"] = id;
    j["qrCodeNo"] = qrCodeNo;
    j["qrCodeUrl"] = qrCodeUrl;
    j["status"] = status;
    j["token"] = token;
    j["updatedTime"] = updatedTime;
    j["userId"] = userId;
    
    return j;
}

bool GBSQrCodeResponse::operator==(const GBSQrCodeResponse& other) const noexcept {
    return account == other.account &&
           createdTime == other.createdTime &&
           endTime == other.endTime &&
           id == other.id &&
           qrCodeNo == other.qrCodeNo &&
           qrCodeUrl == other.qrCodeUrl &&
           status == other.status &&
           token == other.token &&
           updatedTime == other.updatedTime &&
           userId == other.userId;
}
