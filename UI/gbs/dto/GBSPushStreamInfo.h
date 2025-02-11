#ifndef GBSPUSHSTREAMINFO_H
#define GBSPUSHSTREAMINFO_H

#include <string>
#include "nlohmann/json.hpp"

class GBSPushStreamInfo {
public:
    GBSPushStreamInfo();
    virtual ~GBSPushStreamInfo();


        // Getters
    int getId() const;
    int getLiveAccountId() const;
    int getLiveServerId() const;
    std::string getPushStreamUrl() const;
    int getStreamSource() const;
    int getUserId() const;

    // Setters
    void setId(int id);
    void setLiveAccountId(int liveAccountId);
    void setLiveServerId(int liveServerId);
    void setPushStreamUrl(const std::string& pushStreamUrl);
    void setStreamSource(int streamSource);
    void setUserId(int userId);

    static GBSPushStreamInfo fromJson(const std::string& json);
    static std::list<GBSPushStreamInfo> fromJsonArray(const std::string& json);
	nlohmann::json toJson() const;

    bool operator==(const GBSPushStreamInfo &other) const noexcept;

private:

    
    int id;
    int liveAccountId;
    int liveServerId;
    std::string pushStreamUrl;
    int streamSource;
    int userId;
};

#endif // GBSPUSHSTREAMINFO_H

