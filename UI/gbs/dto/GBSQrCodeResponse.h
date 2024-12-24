#ifndef __GBS_QR_CODE_RESPONSE_H__
#define __GBS_QR_CODE_RESPONSE_H__
#include <stdio.h>
#include <string>

 #include "nlohmann/json.hpp"



class GBSQrCodeResponse {
public:
	GBSQrCodeResponse() = default;

	    // 带参数的构造函数
    GBSQrCodeResponse(
        const std::string& account,
        const std::string& createdTime,
        const std::string& endTime,
        int id,
        const std::string& qrCodeNo,
        const std::string& qrCodeUrl,
        int status,
        const std::string& token,
        const std::string& updatedTime,
        int userId
    ) : account(account),
        createdTime(createdTime),
        endTime(endTime),
        id(id),
        qrCodeNo(qrCodeNo),
        qrCodeUrl(qrCodeUrl),
        status(status),
        token(token),
        updatedTime(updatedTime),
        userId(userId)
    {}

	// Getters
	const std::string& getAccount() const { return account; }
	const std::string& getCreatedTime() const { return createdTime; }
	const std::string& getEndTime() const { return endTime; }
	int getId() const { return id; }
	const std::string& getQrCodeNo() const { return qrCodeNo; }
	const std::string& getQrCodeUrl() const { return qrCodeUrl; }
	int getStatus() const { return status; }
	const std::string& getToken() const { return token; }
	const std::string& getUpdatedTime() const { return updatedTime; }
	int getUserId() const { return userId; }

	// Setters
	void setAccount(const std::string& value) { account = value; }
	void setCreatedTime(const std::string& value) { createdTime = value; }
	void setEndTime(const std::string& value) { endTime = value; }
	void setId(int value) { id = value; }
	void setQrCodeNo(const std::string& value) { qrCodeNo = value; }
	void setQrCodeUrl(const std::string& value) { qrCodeUrl = value; }
	void setStatus(int value) { status = value; }
	void setToken(const std::string& value) { token = value; }
	void setUpdatedTime(const std::string& value) { updatedTime = value; }
	void setUserId(int value) { userId = value; }


	static GBSQrCodeResponse fromJson(const std::string& json);
	nlohmann::json toJson() const;

    bool operator==(const GBSQrCodeResponse &other) const noexcept;

private:
	std::string account;
	std::string createdTime;
	std::string endTime;
	int id;
	std::string qrCodeNo;
	std::string qrCodeUrl;
	int status;
	std::string token;
	std::string updatedTime;
	int userId;

	

};
#endif //__GBS_QR_CODE_RESPONSE_H__
