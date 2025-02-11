#ifndef GBSLIVETRANSCRIBE_H
#define GBSLIVETRANSCRIBE_H

#include <string>
#include "nlohmann/json.hpp"

class GBSLiveTranscribe {
public:
    GBSLiveTranscribe();
    virtual ~GBSLiveTranscribe();


    // Getters
    std::string getCreatedTime() const;
    std::string getFileName() const;
    std::string getFileSize() const;
    std::string getFileUrl() const;
    int getId() const;
    int getIsDel() const;
    std::string getLiveRoom() const;
    std::string getPeriodValidity() const;
    int getStreamLogId() const;
    double getTranscribeDuration() const;
    int getTranscribeNo() const;

    // Setters
    void setCreatedTime(const std::string& createdTime);
    void setFileName(const std::string& fileName);
    void setFileSize(const std::string& fileSize);
    void setFileUrl(const std::string& fileUrl);
    void setId(int id);
    void setIsDel(int isDel);
    void setLiveRoom(const std::string& liveRoom);
    void setPeriodValidity(const std::string& periodValidity);
    void setStreamLogId(int streamLogId);
    void setTranscribeDuration(double transcribeDuration);
    void setTranscribeNo(int transcribeNo);


    static GBSLiveTranscribe fromJson(const std::string& json);
    static std::list<GBSLiveTranscribe> fromJsonArray(const std::string& json);
	nlohmann::json toJson() const;

    bool operator==(const GBSLiveTranscribe &other) const noexcept;

private:

    std::string createdTime;
    std::string fileName;
    std::string fileSize;
    std::string fileUrl;
    int id;
    int isDel;
    std::string liveRoom;
    std::string periodValidity;
    int streamLogId;
    double transcribeDuration;
    int transcribeNo;
};

#endif // GBSLIVETRANSCRIBE_H

