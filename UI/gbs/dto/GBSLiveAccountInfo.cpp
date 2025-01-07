#include "GBSLiveAccountInfo.h"

GBSLiveAccountInfo::GBSLiveAccountInfo(
    std::string activationCode,
    int activationStatus,
    std::string customerNo,
    std::string deviceCode,
    std::string deviceNo,
    std::string head,
    int id,
    std::string liveAccount,
    int liveDeviceId,
    std::string livePlatform,
    int liveServerId,
    std::string motherBoarderNo,
    std::string nickname,
    std::string notes,
    std::string platformAccount,
    std::string productNo,
    int remoteSwitch,
    std::string toDeskAccount,
    std::string toDeskPassword,
    int userId,
    std::string version
) : activationCode(std::move(activationCode)),
    activationStatus(activationStatus),
    customerNo(std::move(customerNo)),
    deviceCode(std::move(deviceCode)),
    deviceNo(std::move(deviceNo)),
    head(std::move(head)),
    id(id),
    liveAccount(liveAccount),
    liveDeviceId(std::move(liveDeviceId)),
    livePlatform(std::move(livePlatform)),
    liveServerId(liveServerId),
    motherboardNo(motherBoarderNo),
    nickname(std::move(nickname)),
    notes(std::move(notes)),
    platformAccount(std::move(platformAccount)),
    productNo(std::move(productNo)),
    remoteSwitch(remoteSwitch),
    toDeskAccount(std::move(toDeskAccount)),
    toDeskPassword(std::move(toDeskPassword)),
    userId(userId),
    version(std::move(version))
{}

GBSLiveAccountInfo& GBSLiveAccountInfo::operator=(const GBSLiveAccountInfo& other) {
    if (this != &other) {
        activationCode = other.activationCode;
        activationStatus = other.activationStatus;
        customerNo = other.customerNo;
	deviceCode = other.deviceCode;
        deviceNo = other.deviceNo;
        head = other.head;
        id = other.id;
        liveAccount = other.liveAccount;
        liveDeviceId = other.liveDeviceId;
        livePlatform = other.livePlatform;
        liveServerId = other.liveServerId;
	motherboardNo = other.motherboardNo;
        nickname = other.nickname;
        notes = other.notes;
        platformAccount = other.platformAccount;
        productNo = other.productNo;
        toDeskAccount = other.toDeskAccount;
        toDeskPassword = other.toDeskPassword;
        userId = other.userId;
        version = other.version;
    }
    return *this;
}

GBSLiveAccountInfo GBSLiveAccountInfo::fromJson(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    
    // 所有变量都添加 null 检查
    std::string activationCode = j["activationCode"].is_null() ? "" : j["activationCode"].get<std::string>();
    int activationStatus = j["activationStatus"].is_null() ? 0 : j["activationStatus"].get<int>();
    std::string customerNo = j["customerNo"].is_null() ? "" : j["customerNo"].get<std::string>();
    std::string deviceCode = j["deviceCode"].is_null() ? "" : j["deviceCode"].get<std::string>();
    std::string deviceNo = j["deviceNo"].is_null() ? "" : j["deviceNo"].get<std::string>();
    std::string head = j["head"].is_null() ? "" : j["head"].get<std::string>();
    int id = j["id"].is_null() ? 0 : j["id"].get<int>();
    std::string liveAccount = j["liveAccount"].is_null() ? 0 : j["liveAccount"].get<std::string>();
    int liveDeviceId = j["liveDeviceId"].is_null() ? 0 : j["liveDeviceId"].get<int>();
    std::string livePlatform = j["livePlatform"].is_null() ? "" : j["livePlatform"].get<std::string>();
    int liveServerId = j["liveServerId"].is_null() ? 0 : j["liveServerId"].get<int>();
    std::string motherboardNo = j["motherboardNo"].is_null() ? 0 : j["motherboardNo"].get<std::string>();
    std::string nickname = j["nickname"].is_null() ? "" : j["nickname"].get<std::string>();
    std::string notes = j["notes"].is_null() ? "" : j["notes"].get<std::string>();
    std::string platformAccount = j["platformAccount"].is_null() ? "" : j["platformAccount"].get<std::string>();
    std::string productNo = j["productNo"].is_null() ? "" : j["productNo"].get<std::string>();
    int remoteSwitch = j["remoteSwitch"].is_null() ? 0 : j["remoteSwitch"].get<int>();
    std::string toDeskAccount = j["toDeskAccount"].is_null() ? "" : j["toDeskAccount"].get<std::string>();
    std::string toDeskPassword = j["toDeskPassword"].is_null() ? "" : j["toDeskPassword"].get<std::string>();
    int userId = j["userId"].is_null() ? 0 : j["userId"].get<int>();
    std::string version = j["version"].is_null() ? "" : j["version"].get<std::string>();
    
    // 使用声明的变量构造并返回对象
    return GBSLiveAccountInfo(
        activationCode,
        activationStatus,
        customerNo,
	deviceCode,
        deviceNo,
        head,
        id,
        liveAccount,
        liveDeviceId,
        livePlatform,
        liveServerId,
	motherboardNo,
        nickname,
        notes,
        platformAccount,
        productNo,
        remoteSwitch,
        toDeskAccount,
        toDeskPassword,
        userId,
        version
    );
}

nlohmann::json GBSLiveAccountInfo::toJson() const {
    return nlohmann::json{
        {"activationCode", activationCode},
        {"activationStatus", activationStatus},
        {"customerNo", customerNo},
        {"deviceNo", deviceNo},
        {"head", head},
        {"id", id},
        {"liveAccount", liveAccount},
        {"liveDeviceId", liveDeviceId},
        {"livePlatform", livePlatform},
        {"liveServerId", liveServerId},
        {"nickname", nickname},
        {"notes", notes},
        {"platformAccount", platformAccount},
        {"productNo", productNo},
        {"toDeskAccount", toDeskAccount},
        {"toDeskPassword", toDeskPassword},
        {"userId", userId},
        {"version", version}
    };
}

bool GBSLiveAccountInfo::operator==(const GBSLiveAccountInfo& other) const noexcept {
    return activationCode == other.activationCode
        && activationStatus == other.activationStatus
        && customerNo == other.customerNo
        && deviceNo == other.deviceNo
        && head == other.head
        && id == other.id
        && liveAccount == other.liveAccount
        && liveDeviceId == other.liveDeviceId
        && livePlatform == other.livePlatform
        && liveServerId == other.liveServerId
        && nickname == other.nickname
        && notes == other.notes
        && platformAccount == other.platformAccount
        && productNo == other.productNo
        && toDeskAccount == other.toDeskAccount
        && toDeskPassword == other.toDeskPassword
        && userId == other.userId
        && version == other.version;
}
