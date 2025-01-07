#ifndef __GBS_BUNDLE_DATA_H__
#define __GBS_BUNDLE_DATA_H__

#include <string>
#include <iostream>
#include "nlohmann/json.hpp"

class GBSBundleData {
public:
    // 默认构造函数
    GBSBundleData() = default;

    // 带参数的构造函数
    GBSBundleData(const std::string &activateId, const std::string &deviceId, const std::string &productId,
                  const std::string &uniqueIid)
        : ActivateId(activateId),
          DeviceID(deviceId),
          ProductID(productId),
          UniqueIID(uniqueIid)
    {
    }

    // 拷贝构造函数
    GBSBundleData(const GBSBundleData &other)
        : ActivateId(other.ActivateId),
          DeviceID(other.DeviceID),
          ProductID(other.ProductID),
          UniqueIID(other.UniqueIID)
    {
    }

    // 赋值运算符重载
    GBSBundleData& operator=(const GBSBundleData &other) {
        if (this != &other) {
            ActivateId = other.ActivateId;
            DeviceID = other.DeviceID;
            ProductID = other.ProductID;
            UniqueIID = other.UniqueIID;
        }
        return *this;
    }

    // 获取和设置 ActivateId
    std::string getActivateId() const { return ActivateId; }
    void setActivateId(const std::string &activateId) { ActivateId = activateId; }

    // 获取和设置 DeviceID
    std::string getDeviceID() const { return DeviceID; }
    void setDeviceID(const std::string &deviceId) { DeviceID = deviceId; }

    // 获取和设置 ProductID
    std::string getProductID() const { return ProductID; }
    void setProductID(const std::string &productId) { ProductID = productId; }

    // 获取和设置 UniqueIID
    std::string getUniqueIID() const { return UniqueIID; }
    void setUniqueIID(const std::string &uniqueIid) { UniqueIID = uniqueIid; }

    // 序列化函数：将 GBSBundleData 对象转为 JSON 字符串
    std::string serialize() const
    {
        nlohmann::json j;
        j["ActivateId"] = ActivateId;
        j["DeviceID"] = DeviceID;
        j["ProductID"] = ProductID;
        j["UniqueIID"] = UniqueIID;
        return j.dump();
    }

    // 反序列化函数：从 JSON 字符串转为 GBSBundleData 对象
    static GBSBundleData deserialize(const std::string &jsonData)
    {
        GBSBundleData data;
        try {
            auto jsonObject = nlohmann::json::parse(jsonData);
            data.ActivateId = jsonObject["ActivateId"];
            data.DeviceID = jsonObject["DeviceID"];
            data.ProductID = jsonObject["ProductID"];
            data.UniqueIID = jsonObject["UniqueIID"];
        } catch (const nlohmann::json::exception &e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
        return data;
    }

private:
	std::string ActivateId{""};
	std::string DeviceID{""};
	std::string ProductID{""};
	std::string UniqueIID{""};
	
};

#endif //__GBS_BUNDLE_DATA_H__
