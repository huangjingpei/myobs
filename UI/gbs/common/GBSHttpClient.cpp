#include "gbs/common/GBSHttpClient.h"
#include <iostream>
#include <windows.h>
#include <iostream>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "gbs/common/SystemUtils.h"
#include <fstream>
extern "C" {
	#include "gbs/common/SystemUtils.h"
}
#include "gbs/common/QBizLogger.h"
using json = nlohmann::json;

#include <QRunnable>
#include <QThreadPool>
#include <QDebug>
#include "gbs/common/QBizLogger.h"
#include "gbs/dto/GBSQrCodeResponse.h"
#include "gbs/dto/GBSLiveAccountInfo.h"
#include "gbs/dto/GBSLiveDevices.h"
#include "gbs/GBSMainCollector.h"
#define DEBUG_MSG

class HttpRequestTask : public QRunnable {
public:
	template<typename RequestFunc, typename... Args>
	HttpRequestTask(
		RequestFunc &&requestFunc,
		const std::function<void(const std::string &)> &responseFunc,
		Args &&...args)
		: requestFunc(std::bind(std::forward<RequestFunc>(requestFunc),
					std::forward<Args>(args)...)),
		  responseFunc(responseFunc)
	{
	}

	void run() override
	{
		if (requestFunc) {
			std::string result = requestFunc(); // 执行请求
			if (responseFunc) {
#ifdef DEBUG_MSG
				qDebug() << "DEBUG_MSG recv:" << result;
#endif
				responseFunc(result); // 执行响应处理
			}
		}
	}

private:
	std::function<std::string()> requestFunc;              // 请求行为
	std::function<void(const std::string &)> responseFunc; // 响应处理行为
};


GBSHttpClient::GBSHttpClient()
{
	baseUrl = GBSMainCollector::getInstance()->getBaseUrl();
	baseUrlV2 = GBSMainCollector::getInstance()->getBaseUrlV2();
	QLogD("Http client start");
	do {
		size_t pos = baseUrl.find("https://");
		if (pos != std::string::npos) {
			httpHost = baseUrl.substr(
				pos + 8); // 加7是因为"http://"有7个字符
			
		}

		pos = baseUrl.find("http://");
		if (pos != std::string::npos) {
			httpHost = baseUrl.substr(
				pos + 7); // 加7是因为"http://"有7个字符
			
		}

		size_t posV2 = baseUrlV2.find("https://");
		if (posV2 != std::string::npos) {
			httpHostV2 = baseUrlV2.substr(
				posV2 + 8); // 加7是因为"http://"有7个字符
			
		}

		posV2 = baseUrlV2.find("http://");
		if (posV2 != std::string::npos) {
			httpHostV2 = baseUrlV2.substr(
				posV2 + 7); // 加7是因为"http://"有7个字符
			
		}


	} while (0);
	QLogD("Http client start a thread");
	executor = std::make_unique<TaskExecutor>("SignalThread");
}

std::string GBSHttpClient::getDeviceNo()
{
	std::string productId =
		GetWindowsProductIDFromRegistery();
	std::string deviceId =
		GetMachineIdFromRegistry();
	if (productId.empty() || deviceId.empty()) {
		std::cerr << "product Id is "
			  << "[" << productId << "]"
			  << "device Id is "
			  << "[" << deviceId << "]" << std::endl;
		return "";
	}
	qDebug() << " productID " << productId;
	if (strncmp("00331-10000-00001-AA498", productId.c_str(),23) == 0) {
		productId = "00331-10000-00001-AA819";
	}
	

	std::string deviceNo = deviceId.substr(0, deviceId.size() - 1) +
			       productId; //"{" + deviceId + "}" + productId;
	return deviceNo;
}

std::string GBSHttpClient::getDeviceNoWithoutBraces() {
	std::string productId = GetWindowsProductIDFromRegistery();
	std::string deviceId = GetMachineIdFromRegistry();
	if (productId.empty() || deviceId.empty()) {
		std::cerr << "product Id is "
			  << "[" << productId << "]"
			  << "device Id is "
			  << "[" << deviceId << "]" << std::endl;
		return "";
	}
	qDebug() << " productID " << productId;
	if (strncmp("00331-10000-00001-AA498", productId.c_str(), 23) == 0) {
		productId = "00331-10000-00001-AA819";
	}

	std::string deviceNo = deviceId.substr(1, deviceId.size() - 3) + productId; //"{" + deviceId + "}" + productId;
	return deviceNo;
}

void GBSHttpClient::registerHandler(OBSHttpEventHandler* handler) {
	std::lock_guard<std::mutex> guard(cs);
	handlers.push_back(handler);
}
void GBSHttpClient::unRegisterHandler(OBSHttpEventHandler* handler) {
	std::lock_guard<std::mutex> guard(cs);
	handlers.remove_if([handler](OBSHttpEventHandler *item) {
		return item == handler;
	});
}
void GBSHttpClient::loginWithCheckVersion(
	std::string phone, std::string password, int loginType,
					  std::string vendor)

{
	executor->addTask(
		[phone, password, loginType, vendor, this]() {
			loginWithCheckVersionTask(phone, password, loginType,
						  vendor);
		});
	
}

void GBSHttpClient::loginWithCheckVersionTask(
	std::string phone, std::string password, int loginType,
					  std::string vendor)

{
	QLogD("Login...");


	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		{
			std::lock_guard<std::mutex> guard(cs);
			for (auto it = handlers.begin(); it != handlers.end();
			     it++) {
				(*it)->onLoginResult(-4);
			}
			return;
		}
		
	}
	json body = {{"deviceNo", deviceNo}, {"type", loginType},
		     {"phone", phone},       {"code", ""},
		     {"password", password}, {"versionNo", vendor}};

	std::string url = baseUrl + "/preferred/login/checkVersionLogin";
	auto task = new HttpRequestTask(
		[this](std::string url, json body) -> std::string {
			auto r = cpr::PostAsync(
					 cpr::Url{url}, cpr::Body{body.dump()},
					 cpr::Header{
						 {"Content-Type",
						  "application/json"},
						 {"host",
						  httpHost},
						 {"accept-encoding", "gzip"}},
					 cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				QLogD("Login received 200 OK");
				return r.text;

			} else {
				std::cerr
					<< "[ERROR] unable to connect transport"
					<< " [status code:" << r.status_code
					<< ", body:\"" << r.text << "\"]"
					<< std::endl;

			QLogE("Login: unable to connect transport status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r["result"]["token"].is_null() &&
				    r["result"]["token"].is_string()) {
					this->token =
						r["result"]["token"]
							.get<std::string>();
					{
						std::lock_guard<std::mutex>
							guard(cs);
						for (auto it = handlers.begin();
						     it != handlers.end();
						     it++) {
							(*it)->onLoginResult(0);
						}
						QLogD("Login success.");

						return;
					}
				} else {
					//回复消息内容错误
					{
						std::lock_guard<std::mutex>
							guard(cs);
						for (auto it = handlers.begin();
						     it != handlers.end();
						     it++) {
							(*it)->onLoginResult(-3);
						}
						QLogD("Response text invalid. received msg:%s.", response.c_str());
						return;
					}
				}
			} else {
				//服务器拒绝
				{
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin();
					     it != handlers.end(); it++) {
						(*it)->onLoginResult(-1);
					}
					return;
				}
			}
		},
		url, body);
	task->run();
	
}

void GBSHttpClient::addBarrageRoomConfig() {

}
void GBSHttpClient::addBarrageRoomConfigTask()
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	json body = {{"deviceNo", deviceNo},       {"type", 5},
		     {"phone", "hjp9221@163.com"}, {"code", ""},
		     {"password", "jp517199"},     {"versionNo", "多多客"}};

	auto r =
		cpr::PostAsync(
			cpr::Url{
				this->baseUrl +
				"/preferred/remoteProxyBroadcasting/addBarrageRoomConfig"},
			cpr::Body{body.dump()},
			cpr::Header{{"Content-Type", "application/json"},
				    {"host", httpHost},
				    {"accept-encoding", "gzip"}},
			cpr::VerifySsl{false})
			.get();

	if (r.status_code == 200) {
		auto response = json::parse(r.text);
		token = response["result"]["token"].get<std::string>();
	} else {
		std::cerr << "[ERROR] unable to connect transport"
			  << " [status code:" << r.status_code << ", body:\""
			  << r.text << "\"]" << std::endl;
	}
	return;

}

void GBSHttpClient::getPullStream() {
	executor->addTask([this]() { this->getPullStreamTask(); });
}
void GBSHttpClient::getPullStreamTask()
{
	std::string deviceNo = getDeviceNoWithoutBraces();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	json body = {{"equipmentNo", deviceNo}};


	std::string url = baseUrl + "/preferred/srsLive/getPullStream";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] get pull stream"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if ((!r["result"].is_null()) && (r["result"].is_string())) {
					{
						std::string rtmpUrl = r["result"].get<std::string>();
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onPullRtmpUrl(rtmpUrl);
						}
						return;
					}
				}
			} else {
				std::cerr << "[ERROR] get pull stream, please check the respose:" << response
					  << std::endl;
			}
		},
		url, body.dump(), token);
	task->run();
}
void GBSHttpClient::checkDeviceNoCreateQrCodeScan() {
	executor->addTask([this]() { this->checkDeviceNoCreateQrCodeScanTask(); });
}

void GBSHttpClient::checkDeviceNoCreateQrCodeScanTask()
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::VerifyHost{false},
					   cpr::ssl::VerifyPeer{false});

	cpr::Url url{this->baseUrl +
		     "/preferred/user/checkDeviceNoCreateQrCodeScan/" +
		     cpr::util::urlEncode(deviceNo)};
	auto r = cpr::Get(url, sslOpts);
	if (r.status_code == 200) {
		auto response = json::parse(r.text);
		if (response["result"].is_object()) {
			qrCodeBuf.createTime = response["result"]["createdTime"]
						       .get<std::string>();
			qrCodeBuf.updateTime = response["result"]["updatedTime"]
						       .get<std::string>();
			qrCodeBuf.endTime =
				response["result"]["endTime"].get<std::string>();
			std::string qrCodeUrl = response["result"]["qrCodeUrl"]
							.get<std::string>();
			cpr::Url url{qrCodeUrl};
			cpr::Session session;
			session.SetHeader(
				cpr::Header{{"Accept-Encoding", "gzip"}});
			session.SetUrl(url);
			cpr::cpr_off_t length = session.GetDownloadFileLength();
			qrCodeBuf.fileBuf = (char *)realloc(qrCodeBuf.fileBuf,
							    (long long)length);
			cpr::Response response = session.Download(cpr::WriteCallback{
				[](const std::string_view &data,
				   intptr_t userdata) {
					struct MemFile *file =
						(struct MemFile *)(userdata);
					memcpy((char *)(file->fileBuf) +
						       file->readLen,
					       data.data(), data.size());
					file->readLen += data.size();
					return true; // 返回 true 表示继续写入
				},
				reinterpret_cast<intptr_t>(
					&qrCodeBuf) // userdata
			});
		}
	} else {
		std::cerr << "[ERROR] unable to connect transport"
			  << " [status code:" << r.status_code << ", body:\""
			  << r.text << "\"]" << std::endl;
	}

}


void GBSHttpClient::sendWebsocketMsg(std::string msg)
{
	std::string plat = GBSMainCollector::getInstance()->getDanmakuPlat();
	if (plat.empty()) {
		return;
	}
	executor->addTask([msg, this]() { this->sendWebsocketMsgTask(msg); });
}
void GBSHttpClient::sendWebsocketMsgTask(std::string msg) {
	json body = {{"info", msg}};
	std::string url = baseUrl + "/preferred/distribute/sendAdminDistributeGoodsWebSocketInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body,
		       std::string token) -> std::string {
			auto r = cpr::PostAsync(
					 cpr::Url{url}, cpr::Body{body},
					 cpr::Header{
						 {"Content-Type",
						  "application/json"},
						 {"host",
						  httpHost},
						 {"token", token}},
					 cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code
					  << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code,
				      r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::queryByEquipmentNo() {
	executor->addTask([this]() { this->queryByEquipmentNoTask(); });
	
}
void GBSHttpClient::queryByEquipmentNoTask()
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	json body = {{"equipmentNo", deviceNo}};

	std::string url = baseUrl + "/preferred/remoteProxyBroadcasting/queryByEquipmentNo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body,
		       std::string token) -> std::string {
			auto r = cpr::PostAsync(
					 cpr::Url{url}, cpr::Body{body},
					 cpr::Header{
						 {"Content-Type",
						  "application/json"},
						 {"host",
						  httpHost},
						 {"token", token}},
					 cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] query by equipment"
					  << " [status code:" << r.status_code
					  << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if ((!r["result"].is_null()) &&
				    (r["result"].is_object())) {
					equipmentInfo = GBSRoomInfo::
						fromJsonByEquipment(
						response);
					{
						std::lock_guard<std::mutex>
							guard(cs);
						for (auto it = handlers.begin();
						     it != handlers.end();
						     it++) {
							(*it)->onRoomInfo(
								&equipmentInfo);
						}
						return;
					}
				}
			} else {
				std::cerr
					<< "[ERROR] getquery by equipment, please check the respose:"
					<< response << std::endl;
			}
			
		},
		url, body.dump(), token);
	task->run();
	
		

	
}


void GBSHttpClient::getUserInfo() {
	executor->addTask(
		[this]() { this->getUserInfoTask();
		});
}
void GBSHttpClient::getUserInfoTask()
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	json body = {{"equipmentNo", deviceNo}};

	std::string url = baseUrl + "/preferred/user/userInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body,
		       std::string token) -> std::string {
			auto r = cpr::PostAsync(
					 cpr::Url{url}, cpr::Body{body},
					 cpr::Header{
						 {"Content-Type",
						  "application/json"},
						 {"host",
						  httpHost},
						 {"token", token}},
					 cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] get user info"
					  << " [status code:" << r.status_code
					  << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if ((!r["result"].is_null()) &&
				    (r["result"].is_object())) {
					GBSUserInfo userInfo = GBSUserInfo::fromJson(
							response);
					std::string headerUrl =
						userInfo.GetHead();
					{
						    std::lock_guard<std::mutex>
							    guard(cs);
						    for (auto it =
								 handlers.begin();
							 it != handlers.end();
							 it++) {
							    (*it)->onUserInfo(
								    &userInfo);
						    }
						    return;
					}
					
				}
			} else {
				std::cerr
					<< "[ERROR] get user info, please check the respose:"
					<< response << std::endl;
			}
		},
		url, body.dump(), token);
	task->run();

	
}
void GBSHttpClient::downFile(std::string url, std::string path, int type) {
	executor->addTask(std::bind(&GBSHttpClient::downFileTask, this, url, path, type));
}

void GBSHttpClient::downFileTask(std::string url, std::string pathfile, int type)
{
	auto task = new HttpRequestTask(
		[this](std::string url, std::string pathfile, int type) -> std::string {
			cpr::Url cprUrl{url};
			cpr::Session session;
			session.SetHeader(cpr::Header{{"Accept-Encoding", "gzip"}});
			session.SetUrl(cprUrl);
			std::filesystem::path exePath = std::filesystem::current_path();
			std::filesystem::path filePath = exePath / pathfile;

			std::FILE *file = std::fopen(filePath.string().c_str(), "wb");
			cpr::Response response = session.Download(cpr::WriteCallback{
				[](const std::string_view &data, intptr_t userdata) {
					// 假设 userdata 是文件指针或者内存指针，用来保存数据
					std::FILE *file = reinterpret_cast<std::FILE *>(userdata);
					if (file) {
						std::fwrite(data.data(), 1, data.size(), file);
					}
					return data.size(); // 返回处理的数据大小，表示成功写入
				},
				reinterpret_cast<intptr_t>(file) // userdata
			});
			std::fclose(file);
			if (response.status_code == 200 && response.error.code == cpr::ErrorCode::OK) {
				std::cout << "Download succeeded!" << std::endl;
				{
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onUserFileDownLoad(filePath.string().c_str(), type);
					}
					return "OK";
				}

			} else {
				std::cerr << "Download failed! Status code: " << response.status_code << std::endl;
				QLogE("Download %s failed! Status code:%d", url.c_str(), response.status_code);
				return "Error";
			}
		},
				    [this](const std::string &response) {

				    }, url, pathfile, type);


			task->run();
	return;
}



void GBSHttpClient::pageQuery() {
	executor->addTask(std::bind(&GBSHttpClient::pageQueryTask, this));
}

void GBSHttpClient::pageQueryTask()
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	json body = {{"pageNum", 1}, {"pageSize", 50}};
	std::string url = baseUrl + "/preferred/remoteProxyBroadcasting/pageQuery";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body,
		       std::string token) -> std::string {
			qDebug() << __FILE__ << ":" << __FUNCTION__ << ":"
				 << __LINE__;
			auto r = cpr::PostAsync(
					 cpr::Url{url}, cpr::Body{body},
					 cpr::Header{
						 {"Content-Type",
						  "application/json"},
						 {"host",
						  httpHost},
						 {"token", token}},
					 cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				qDebug() << __FILE__ << ":" << __FUNCTION__
					 << ":" << __LINE__;
				return r.text;


			} else {
				std::cerr << "[ERROR] page Query error:"
					  << " [status code:" << r.status_code
					  << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		}, [this](const std::string &response) {
			if (response.compare("Error")) {
				qDebug() << __FILE__ << ":" << __FUNCTION__
					 << ":" << __LINE__;
				auto r = json::parse(response);
				if ((!r["result"].is_null()) &&
				    (r["result"].is_object())) {
					{
						qDebug() << __FILE__ << ":"
							 << __FUNCTION__ << ":"
							 << __LINE__;
						std::lock_guard<std::mutex> guard(cs);

						roomsInfos.clear();
						roomsInfos =
							GBSRoomInfo::fromJson(response);
			

						for (auto it = handlers.begin();
						     it != handlers.end(); it++) {
							(*it)->onRoomInfos(roomsInfos);
						}
						return;
					}

				}


			}
			else {
				qDebug() << __FILE__ << ":" << __FUNCTION__
					 << ":" << __LINE__;
			std::cerr
				<< "[ERROR] page qeury failed, please check the respose:"
				<< response << std::endl;
			}
			

		},
		url, body.dump(), token);


		task->run();
	return;

}

void GBSHttpClient::destroy() {
	executor.reset();
}

void GBSHttpClient::startLive(std::string &equipments)
{
	executor->addTask(
		std::bind(&GBSHttpClient::startLiveTask, this, equipments));
}

void GBSHttpClient::startLiveTask(std::string &equipments)
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	std::string url = baseUrl + "/preferred/srsLive/startLive";

	json body = {{"equipmentNo", equipments}};

		auto task =
		new HttpRequestTask([this](std::string url, std::string body,
					   std::string token) -> std::string {
			auto r =
				cpr::PostAsync(
					cpr::Url{
						this->baseUrl +
						"/preferred/srsLive/startLive"},
					cpr::Body{body},
					cpr::Header{{"Content-Type",
						     "application/json"},
						    {"host",
						     httpHost},
						    {"token", token}},
					cpr::VerifySsl{false})
					.get();
			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] start live error:"
					  << " [status code:" << r.status_code
					  << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}

			},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if ((!r["result"].is_null()) &&
				    (r["result"].is_string())) {
					
					std::string rtmpPushUrl =
						r["result"].get<std::string>();
					if (rtmpPushUrl.empty()) {
						return;
					}

					{
						std::lock_guard<std::mutex>
							guard(cs);
						for (auto it = handlers.begin();
						     it != handlers.end();
						     it++) {
							(*it)->onRtmpPushUrl(
								rtmpPushUrl, 0);
						}
						return;
					}

				} else {
					std::cerr
						<< "[ERROR] start live failed, please check the respose:"
						<< response << std::endl;
				}
			}
			
			}, url, body.dump(),
		token);

		task->run();


		
}

void GBSHttpClient::endLive(std::string id) {
	executor->addTask(std::bind(&GBSHttpClient::endLiveTask, this, id));
}
void GBSHttpClient::endLiveTask(std::string id)
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check." << std::endl;
		return;
	}
	std::string url = baseUrl + "/preferred/srsLive/closeStream";

	json body = {{"id", id}};

	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url},
						cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] close live error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {

			}
		},
		url, body.dump(), token);

	task->run();
}


void GBSHttpClient::upRemoteLiveRoomState(std::string ids)
{
	executor->addTask(
		std::bind(&GBSHttpClient::upRemoteLiveRoomStateTask, this, ids));
}
void GBSHttpClient::upRemoteLiveRoomStateTask(std::string ids)
{
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check."
			  << std::endl;
		return;
	}
	json body = {{"ids", ids}, {"type", 1}};
	std::string url = baseUrl + "/preferred/remoteProxyBroadcasting/upRemoteLiveRoomState";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token)->std::string {
			auto r =
				cpr::PostAsync(
					cpr::Url{url},
					cpr::Body{body},
					cpr::Header{{"Content-Type",
						     "application/json"},
						    {"host",
						     httpHost},
						    {"token", token}},
					cpr::VerifySsl{false})
					.get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] update live room state error:"
					  << " [status code:" << r.status_code
					  << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
		}, url, body.dump(), token);
	
			
	task->run();

	return;
}

void GBSHttpClient::createQrCodeScan() {
	executor->addTask(std::bind(&GBSHttpClient::createQrCodeScanTask, this));
	}
void GBSHttpClient::createQrCodeScanTask() {
	json body = {{}};
	std::string url = baseUrl + "/preferred/user/createQrCodeScan";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost},
							    },
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				auto response = json::parse(r.text);
				if (!response["result"].is_null() && response["result"].is_object()) {
					std::string qrCodeUrl = response["result"]["qrCodeUrl"].get<std::string>();
					std::string qrCodeNo = response["result"]["qrCodeNo"].get<std::string>();
					int status = response["result"]["status"].is_null()
							     ? 0
							     : response["result"]["status"].get<int>();
					if (!qrCodeUrl.empty()) {
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onQRcodeInfo(qrCodeNo, qrCodeUrl, status);
						}
					}
					
				}
				return "OK";

			} else {
				std::cerr << "[ERROR] update live room state error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {

		}, url, body.dump());

	task->run();

	return;
}

void GBSHttpClient::scanLoginInfo(std::string qrCode) {
	executor->addTask(std::bind(&GBSHttpClient::scanLoginInfoTask, this, qrCode));

    }
void GBSHttpClient::scanLoginInfoTask(std::string qrCode) {
	json body = {{"qrCodeNo", qrCode}};
	std::string url = baseUrl + "/preferred/user/scanLoginInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string qrCode) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{
							{"Content-Type", "application/json"},
							{"host", httpHost},
						},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				auto response = json::parse(r.text);
				if (!response["result"].is_null() && response["result"].is_object()) {
					std::string qrCodeUrl = response["result"]["qrCodeUrl"].get<std::string>();
					std::string qrCodeNo = response["result"]["qrCodeNo"].get<std::string>();
					int status = response["result"]["status"].is_null()
							     ? 0
							     : response["result"]["status"].get<int>();
					this->token = response["result"]["token"].is_null()
							      ? ""
							      : response["result"]["token"].get<std::string>();
					if (!qrCodeUrl.empty()) {
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onQRcodeInfo(qrCodeNo, qrCodeUrl, status);
						}
					}
				}
				return "OK";

			} else {
				
				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onQRcodeInfo(qrCode, "", -1);
				}
				
				std::cerr << "[ERROR] update live room state error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {

		},
		url, body.dump(), qrCode);

	task->run();

	return;
}

void GBSHttpClient::memberInfo(std::string userId) {
	executor->addTask(std::bind(&GBSHttpClient::memberInfoTask, this, userId));
}
void GBSHttpClient::memberInfoTask(std::string userId) {
	std::string deviceNo = getDeviceNo();
	if (deviceNo.empty()) {
		std::cerr << "Cannot obtain device id. please check." << std::endl;
		return;
	}
	json body = {{"id", std::stoi(userId)}};
	std::string url = baseUrl + "/preferred/memberLevel/memberInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] retrieve the member info error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
			auto r = json::parse(response);
			if (!r["result"].is_null() && r["result"].is_object()) {
				GBSMemberInfo info = GBSMemberInfo::fromJson(response);
				{
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						//(*it)->onMemberInfo(info);
					}
					QLogD("Login success.");

					return;
				}
			} else {
			
				QLogD("Retrieve member info invalid. received msg:%s.", response.c_str());
				return;
				
			}
		}, url, body.dump(), token);

	task->run();

	return;
}


void GBSHttpClient::codeList(int levelId) {
	executor->addTask(std::bind(&GBSHttpClient::codeListTask, this, levelId));
}
void GBSHttpClient::codeListTask(int levelId) {
	json body = {{"id", levelId}};
	std::string url = baseUrl + "/preferred/memberLevel/codeList";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] retrieve the member info error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {


		},
		url, body.dump(), token);

	task->run();

	return;
}

void GBSHttpClient::remainingActivation(int levelId) {
	executor->addTask(std::bind(&GBSHttpClient::remainingActivationTask, this, levelId));
}
void GBSHttpClient::remainingActivationTask(int levelId) {
	json body = {{"id", levelId}};
	std::string url = baseUrl + "/preferred/memberLevel/remainingActivation";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] retrieve the member info error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[this](const std::string &response) {
			auto r = json::parse(response);
			if (!r.is_null() && r.is_object()) {

			}
		},
		url, body.dump(), token);

	task->run();

	return;
}
void GBSHttpClient::agreement(int type) {
	executor->addTask(std::bind(&GBSHttpClient::agreementTask, this, type));
}
void GBSHttpClient::agreementTask(int type) {
	json body = {{"type", type}};
	std::string url = baseUrl + "/preferred/login/agreement";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, int type) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHost}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				std::cerr << "[ERROR] retrieve the member info error:"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				return "Error";
			}
		},
		[type, this](const std::string &response) {
			auto r = json::parse(response);
			if (!r.is_null() && !r["result"].is_null()) {
				std::string richText = r["result"]["content"].get<std::string>();
				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onAgreementInfo(richText, type);
				}
			}
		},
		url, body.dump(), type);

	task->run();

	return;
}


std::string GBSHttpClient::getEequipmentNo() {
	std::lock_guard<std::mutex> guard(cs);
	return equipmentInfo.getEquipmentNo();
}

void GBSHttpClient::srsScanLoginV2(std::string version) {
	std::string productId = GetWindowsProductIDFromRegistery();
	std::string deviceId = GetMachineIdFromRegistry();
	if (productId.empty() || deviceId.empty()) {
		std::cerr << "product Id is "
			  << "[" << productId << "]"
			  << "device Id is "
			  << "[" << deviceId << "]" << std::endl;
		
		QLogE("[ERROR] srsScanLoginV2 error. device id & product is invalid.");
		return ;
	}
	QLogD("Product Id is %s", productId);
	QLogD("Device Id is %s", deviceId);
	executor->addTask(std::bind(&GBSHttpClient::srsScanLoginTaskV2, this, deviceId, productId, version));
}
void GBSHttpClient::srsScanLoginTaskV2(std::string deviceNo, std::string productNo, std::string version) {
	json body = {
		{"deviceNo", deviceNo},
		{"productNo", productNo},
		{"version", version},
	};
	//std::string url = baseUrlV2 + "/preferred/srs/srsScanLogin";
	std::string url = baseUrlV2 + "/preferred/matrixLive/zlmScanLogin";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"}, {"host", httpHostV2}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] login failed from server.  [status code: %d, body: %s]", r.status_code,
				      r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r.is_null() && !r["result"].is_null() && r["result"].is_object()) {
					GBSQrCodeResponse result = GBSQrCodeResponse::fromJson(r["result"].dump());
					std::string qrCodeUrl = result.getQrCodeUrl();
					std::string qrCodeNo = result.getQrCodeNo();
					this->token = result.getToken();
					int status = result.getStatus();
					qDebug() << "token :" << this->token;
					if (!qrCodeUrl.empty()) {
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onQRcodeInfo(qrCodeNo, qrCodeUrl, status);
						}
					}
					return;
				} else {
	
				}
			} else {

				return;
			}
		},
		url, body.dump());

	task->run();

	return;

}

void GBSHttpClient::srsAccountLoginV2(std::string account, std::string password, std::string smsCode,
				      std::string version, int type)
{
	executor->addTask(std::bind(&GBSHttpClient::srsAccountLoginTaskV2, this, account, password, smsCode, version, type));
}

void GBSHttpClient::srsAccountLoginTaskV2(std::string account, std::string password, std::string smsCode,
					  std::string version, int type)
{
	json jsonBody;
	if ((type == 4) && (!smsCode.empty())) {
		jsonBody = {
			{"account", account}, {"smsCode", smsCode},
			{"version", version},
			{"type", 4}
		};
	} else if ((type == 5) && (!password.empty())) {
		jsonBody = {{"account", account}, {"password", password}, {"version", version}, {"type", 5}};
	} else {
		//参数不正确，报错
		std::lock_guard<std::mutex> guard(cs);
		for (auto it = handlers.begin(); it != handlers.end(); it++) {
			(*it)->onLoginResult(-4);
		}
		QLogE("login failed. invalid argument.");
		return;
	}
	//std::string url = baseUrlV2 + "/preferred/srs/srsAccountLogin";
	std::string url = baseUrlV2 + "/preferred/matrixLive/zlmAccountLogin";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"}, {"host", httpHostV2}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] login failed from server.  [status code: %d, body: %s]", r.status_code, r.error.message.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r.is_null() && !r["result"].is_null() && r["result"]["token"].is_string()) {
					std::string token = r["result"]["token"].get<std::string>();
					this->token = token;
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onLoginResult(0);
					}
					QLogD("Login success.");
					return;
				} else {
					//服务器返回了200OK，但是消息里面没有token
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onLoginResult(-3);
					}

					QLogE("[ERROR] srsAccountLoginTaskV2 [body: %s]", response);

					return;
				}
			} else {
				//服务器拒绝非200OK
				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onLoginResult(-1);
				}
			}
		},
		url, jsonBody.dump());

	task->run();

	return;

}
void GBSHttpClient::scanLoginQrCodeInfoV2(std::string qrCodeNo) {
	executor->addTask(std::bind(&GBSHttpClient::scanLoginQrCodeInfoTaskV2, this, qrCodeNo));
}
void GBSHttpClient::scanLoginQrCodeInfoTaskV2(std::string qrCodeNo) {
	json body = {{"qrCodeNo", qrCodeNo}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/scanLoginQrCodeInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string qrCode) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{
							{"Content-Type", "application/json"},
							{"host", httpHostV2},
						},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				auto response = json::parse(r.text);
				if (!response["result"].is_null() && response["result"].is_object()) {
					std::string qrCodeUrl = response["result"]["qrCodeUrl"].get<std::string>();
					std::string qrCodeNo = response["result"]["qrCodeNo"].get<std::string>();
					int status = response["result"]["status"].is_null()
							     ? 0
							     : response["result"]["status"].get<int>();
					this->token = response["result"]["token"].is_null()
							      ? 0
							      : response["result"]["token"].get<std::string>();
					if (!qrCodeUrl.empty()) {
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onQRcodeInfo(qrCodeNo, qrCodeUrl, status);
						}
					}
				}
				return "OK";

			} else {

				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onQRcodeInfo(qrCode, "", -1);
				}
				QLogE("[ERROR] scanLoginQrCodeInfoTaskV2 [status code: %d, body: %s]", r.status_code,
				      r.text);
				return "Error";
			}
		},
		[this](const std::string &response) {

		},
		url, body.dump(), qrCodeNo);

	task->run();

	return;
}

void GBSHttpClient::srsLiveAccountInfoV2(std::string version)
{
	std::string productId = GetWindowsProductIDFromRegistery();
	std::string deviceId = GetMachineIdFromRegistry();
	std::string boarderNo = GBSMainCollector::getInstance()->getSystemUniqueNo();
	if (productId.empty() || deviceId.empty()) {
		std::cerr << "product Id is "
			  << "[" << productId << "]"
			  << "device Id is "
			  << "[" << deviceId << "]" << std::endl;

		QLogE("[ERROR] srsScanLoginV2 error. device id & product is invalid.");
		return;
	}
	QLogD("Product Id is %s", productId.c_str());
	QLogD("Device Id is %s", deviceId.c_str());
	executor->addTask(
		std::bind(&GBSHttpClient::srsLiveAccountInfoTaskV2, this, deviceId, productId, boarderNo, version));
}
void GBSHttpClient::srsLiveAccountInfoTaskV2(std::string deviceNo, std::string productNo, std::string boarderNo,
					     std::string version)
{
	json body = {
		{"deviceNo", deviceNo},
		{"productNo", productNo},
		{"motherboardNo", boarderNo},
		{"version", version},
	};
	//std::string url = baseUrlV2 + "/preferred/srs/srsLiveAccountInfo";
	std::string url = baseUrlV2 + "/preferred/matrixLive/zlmLiveAccountInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2} ,{"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] login failed from server.  [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				qDebug() << "response:" << response;
				auto r = json::parse(response);
				if (!r.is_null() && !r["result"].is_null() && r["result"].is_object()) {
					GBSLiveAccountInfo result = GBSLiveAccountInfo::fromJson(r["result"].dump());
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onAccountInfo(result);
					}
					return;
				} else {
					//服务器返回了200OK，但是消息里面没有token
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
					}
					QLogE("[ERROR] login failed from server ,token was missing！ [body msg: %s]",
					      response.c_str());
					return;
				}
			} else {
				//服务器拒绝非200OK

				return;
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}


void GBSHttpClient::activateDeviceV2(std::string activationCode, std::string deviceName, std::string deviceNo,
					  int liveAccountId, std::string livePlatform, std::string motherboardNo,
					  std::string notes, std::string platformAccount, std::string productNo,
					  std ::string toDeskAccount, std::string toDeskPassword)
{
	std::string productId = GetWindowsProductIDFromRegistery();
	std::string deviceId = GetMachineIdFromRegistry();
	if (productId.empty() || deviceId.empty()) {
		std::cerr << "product Id is "
			  << "[" << productId << "]"
			  << "device Id is "
			  << "[" << deviceId << "]" << std::endl;

		QLogE("[ERROR] activateDeviceV2 error. device id & product is invalid.");
		return;
	}
	QLogD("Product Id is %s", productId.c_str());
	QLogD("Device Id is %s", deviceId.c_str());
	executor->addTask(std::bind(&GBSHttpClient::activateDeviceTaskV2, this, activationCode, deviceName, deviceNo,
				    liveAccountId, livePlatform, motherboardNo, notes, platformAccount, productId,
				    toDeskAccount,
				    toDeskPassword));
}
void GBSHttpClient::activateDeviceTaskV2(std::string activationCode, std::string deviceName, std::string deviceNo,
					 int liveAccountId, std::string livePlatform, std::string motherboardNo,
					 std::string notes, std::string platformAccount, std::string productNo,
					 std ::string toDeskAccount, std::string toDeskPassword)
{
	json body = {
		{"activationCode", activationCode},
		{"deviceName", deviceName},
		{"deviceNo", deviceNo},
		{"liveAccountId", liveAccountId},   {"productNo", productNo},
		{"livePlatform", livePlatform},
		{"motherboardNo", motherboardNo},
		{"notes", notes},
		{"platformAccount", platformAccount},
		{"toDeskAccount", toDeskAccount},
		{"toDeskPassword", toDeskPassword},
	};
	std::string url = baseUrlV2 + "/preferred/matrixLive/activateDevice";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] activate device failed.  [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r.is_null() && !r["result"].is_null() && r["result"].is_object()) {
					GBSLiveAccountInfo result = GBSLiveAccountInfo::fromJson(r["result"].dump());
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onActiveResult(0);
					}
					return;
				} else {
					//服务器返回了200OK，但是消息里面没有token
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
					}
					QLogE("[ERROR] activate device failed ！ [body msg: %s]",
					      response.c_str());
					return;
				}
			} else {
				//服务器拒绝非200OK
				return;
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}

void GBSHttpClient::sendHeartbeatTimeV2(int userId) {
	executor->addTask(std::bind(&GBSHttpClient::sendHeartbeatTimeTaskV2, this, userId));
}
void GBSHttpClient::sendHeartbeatTimeTaskV2(int userId)
{
	json body = {
		{"id", userId},
	};

#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	std::string url = baseUrlV2 + "/preferred/matrixLive/sendHeartbeatTime";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] send heart beat failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r.is_null()) {
					int result = r["result"].is_null() ? 0 : r["result"].get<int>();
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onHeartBeat(result);
					}
					return;
				} 
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}
void GBSHttpClient::createSrsStreamV2(int streamSource) {

	
	executor->addTask(std::bind(&GBSHttpClient::createSrsStreamTaskV2, this, streamSource));
}
void GBSHttpClient::createSrsStreamTaskV2(int streamSource) {
	json body = {{"streamSource", streamSource}

	};
#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	//std::string url = baseUrlV2 + "/preferred/srs/createSrsStream";
	std::string url = baseUrlV2 + "/preferred/matrixLive/createZlmStream";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] enter control failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r.is_null() && r["result"].is_object()) {
					GBSPushStreamInfo result = GBSPushStreamInfo::fromJson(r["result"].dump());

					int id = result.getId();
					std::string pushStreamUrl = result.getPushStreamUrl();
	
					if (pushStreamUrl.empty()) {
						QLogE("cannot get push stream url from server.");
						return;
					}
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onRtmpPushUrl(pushStreamUrl, id);
					}
					
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onPushStreamInfo(result);
					}
					return;
				} else if (!r.is_null()) {
					int result = r["code"].is_null() ? 0 : r["code"].get<int>();
					if (result < 0) {
						std::string errMsg = r["msg"].is_null() ? ""
											: r["msg"].get<std::string>();
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onRtmpPushError(errMsg);
						}
						return;
					}
				}
			}
		},
		url, body.dump(), token);

	task->run();

	return;

}

void GBSHttpClient::enterControlV2(std::string password, int liveAccountId) {
	executor->addTask(std::bind(&GBSHttpClient::enterControlTaskV2, this, password, liveAccountId));


}
void GBSHttpClient::enterControlTaskV2(std::string password, int liveAccountId) {
	json body = {
		{"controlPassword", password}, {"liveAccountId", liveAccountId}

	};

#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	std::string url = baseUrlV2 + "/preferred/matrixLive/enterControl";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] enter control failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r.is_null()) {
					int result = r["result"].is_null() ? 0 : r["result"].get<int>();
					std::lock_guard<std::mutex> guard(cs);
					for (auto it = handlers.begin(); it != handlers.end(); it++) {
						(*it)->onEnterGuardCtrl(result);
					}
					return;
				}
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}

void GBSHttpClient::pageSrsLiveDeviceV2(int liveAccountId, int pageNum, int pagesize) {
	executor->addTask(std::bind(&GBSHttpClient::pageSrsLiveDeviceTaskV2, this, liveAccountId, pageNum, pagesize));
}
void GBSHttpClient::pageSrsLiveDeviceTaskV2(int liveAccountId, int pageNum, int pagesize) {
	json body = {{"liveAccountId", liveAccountId}, {"pageNum", pageNum}, {"pageSize", pagesize}

	};

#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	//std::string url = baseUrlV2 + "/preferred/srs/pageSrsLiveDevice";
	std::string url = baseUrlV2 + "/preferred/matrixLive/pageZlmLiveDevice";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token, int pageNum) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] query live devices. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[pageNum, this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if (!r["result"].is_null() && !r["result"]["records"].is_null()) {
					if (r["result"]["records"].is_array()) {
						std::list<GBSLiveDevices> devices =
							GBSLiveDevices::fromJsonArray(r["result"]["records"].dump());
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onListDevices(devices, pageNum);
						}

					} else if (r["result"]["records"].is_object()) {
						GBSLiveDevices device =
							GBSLiveDevices::fromJson(r["result"]["records"].dump());
						std::list<GBSLiveDevices> devices;
						devices.push_back(device);
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onListDevices(devices, pageNum);
						}
					}
					return;
				}
			}
		},
		url, body.dump(), token, pageNum);

	task->run();

	return;

}


void GBSHttpClient::addSrsLiveDeviceV2(std::string activationCode, std::string deviceCode, std::string deviceName,
				       int liveAccountId, std::string livePlatform, std::string notes,
				       std::string platformAccount, std::string toDeskAccount,
				       std::string toDeskPassword)
{
	executor->addTask(std::bind(&GBSHttpClient::addSrsLiveDeviceTaskV2, this, activationCode, deviceCode,
				    deviceName,
				    liveAccountId, livePlatform, notes, platformAccount, toDeskAccount,
				    toDeskPassword));
    }


void GBSHttpClient::addSrsLiveDeviceTaskV2(std::string activationCode,
					std::string deviceCode, std::string deviceName,
					int liveAccountId,
					std::string livePlatform, std::string notes, std::string platformAccount,
					std::string toDeskAccount, std::string toDeskPassword)
{
	json body = {
		{"activationCode", activationCode},   {"deviceCode", deviceCode},     {"deviceName", deviceName},
		{"liveAccountId", liveAccountId},   {"livePlatform", livePlatform},   {"notes", notes},
		{"platformAccount", platformAccount},{"toDeskAccount", toDeskAccount},
		{"toDeskPassword", toDeskPassword},

	};

	#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	//std::string url = baseUrlV2 + "/preferred/srs/addSrsLiveDevice";
	std::string url = baseUrlV2 + "/preferred/matrixLive/addZlmLiveDevice";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] enter control failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {

			}
		},
		url, body.dump(), token);

	task->run();

	return;

}

void GBSHttpClient::getPullStreamUrlV2() {
	std::string productId = GetWindowsProductIDFromRegistery();
	std::string deviceId = GetMachineIdFromRegistry();
	std::string motherboardNo = GBSMainCollector::getInstance()->getSystemUniqueNo();
	if (productId.empty() || deviceId.empty()) {
		std::cerr << "product Id is "
			  << "[" << productId << "]"
			  << "device Id is "
			  << "[" << deviceId << "]" << std::endl;

		QLogE("[ERROR] activateDeviceV2 error. device id & product is invalid.");
		return;
	}
	QLogD("Product Id is %s", productId.c_str());
	QLogD("Device Id is %s", deviceId.c_str());
	executor->addTask(std::bind(&GBSHttpClient::getPullStreamUrlTaskV2, this, deviceId, productId, motherboardNo));

    }
void GBSHttpClient::getPullStreamUrlTaskV2(std::string deviceNo, std::string productNo, std::string motherboardNo)
{
	json body = {
		{"deviceNo", deviceNo},
		{"productNo", productNo},
		{"motherboardNo", motherboardNo},
	};

#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	//std::string url = baseUrlV2 + "/preferred/srs/getPullStreamUrl";
	std::string url = baseUrlV2 + "/preferred/matrixLive/getPullStreamUrl";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] get pull stream failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if ((!r["result"].is_null()) && (r["result"].is_string())) {
					{
						std::string rtmpUrl = r["result"].get<std::string>();
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							(*it)->onPullRtmpUrl(rtmpUrl);
						}
						return;
					}
				}
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}

void GBSHttpClient::closeSrsStreamLogV2(int id) {
	executor->addTask(std::bind(&GBSHttpClient::closeSrsStreamLogTaskV2, this, id));
}
void GBSHttpClient::closeSrsStreamLogTaskV2(int id) {
	json body = {{"id", id}};

#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	//std::string url = baseUrlV2 + "/preferred/srs/closeSrsStreamLog";
	std::string url = baseUrlV2 + "/preferred/matrixLive/closeZlmStreamLog";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			QLogE("closeSrsStreamLogTaskV2 %s", body.c_str());
			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] close srs stream failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onPushRtmpClosed();
				}
				return;
			
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}


void GBSHttpClient::deletedSrsLiveDeviceV2(int id) {
	executor->addTask(std::bind(&GBSHttpClient::deletedSrsLiveDeviceTaskV2, this, id));
}
void GBSHttpClient::deletedSrsLiveDeviceTaskV2(int id) {
	json body = {{"id", id}};

#ifdef DEBUG_MSG
	qDebug() << "DEBUG_MSG send:" << body.dump();
#endif
	//std::string url = baseUrlV2 + "/preferred/srs/deletedSrsLiveDevice";
	std::string url = baseUrlV2 + "/preferred/matrixLive/deletedZlmLiveDevice";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();

			if (r.status_code == 200) {
				return r.text;

			} else {
				//服务器直接返回错误信息
				QLogE("[ERROR] delete srs stream failed. [status code: %d, body: %s]", r.status_code,
				      r.text.c_str());
				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
				auto r = json::parse(response);
				if ((!r["result"].is_null()) && (r["result"].is_string())) {
					{
						std::string rtmpUrl = r["result"].get<std::string>();
						std::lock_guard<std::mutex> guard(cs);
						for (auto it = handlers.begin(); it != handlers.end(); it++) {
							//(*it)->onPullRtmpUrl(rtmpUrl);
						}
						return;
					}
				}
			}
		},
		url, body.dump(), token);

	task->run();

	return;
}



void GBSHttpClient::sendWebsocketMsgV2(std::string msg)
{
	executor->addTask(
		[msg, this]() { this->sendWebsocketMsgTaskV2(msg); });
}
void GBSHttpClient::sendWebsocketMsgTaskV2(std::string msg)
{
	json body = {{"info", msg}};
	std::string url = baseUrlV2 + "/preferred/distribute/sendAdminDistributeGoodsWebSocketInfo";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false});
					 //.get();
			//if (r.status_code == 200) {
			//	return r.text;
			//} else {
			//	std::cerr << "[ERROR] send websocket"
			//		  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
			//		  << std::endl;
			//	QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

			//	return "Error";
			//}
			return "OK";
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::modifyZlmLiveDevice(std::string deviceName, int id, std::string livePlatform,
					     std::string notes,
			      std::string platformAccount, int remoteSwitch, std::string toDeskAccount,
			      std::string toDeskPassword)
{
	executor->addTask(std::bind(&GBSHttpClient::modifyZlmLiveDeviceTaskV2, this, deviceName,id,livePlatform, notes, platformAccount, remoteSwitch, toDeskAccount, toDeskPassword));
}

void GBSHttpClient::modifyZlmLiveDeviceTaskV2(std::string deviceName, int id, std::string livePlatform,
						std::string notes,
				std::string platformAccount, int remoteSwitch, std::string toDeskAccount, std::string toDeskPassword)
{
	json body = {{"deviceName", deviceName},
		     {"id", id},
		     {"livePlatform", livePlatform},
		     {"notes", notes},
		     {"platformAccount", platformAccount},
		     {"remoteSwitch", remoteSwitch},
		     {"toDeskAccount", toDeskAccount},
		{"toDeskPassword", toDeskPassword}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/modifyZlmLiveDevice";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::queryZlmLiveDevicesV2(int liveAccountId) {
	executor->addTask(std::bind(&GBSHttpClient::queryZlmLiveDevicesTaskV2, this, liveAccountId));

    }
void GBSHttpClient::queryZlmLiveDevicesTaskV2(int liveAccountId) {
	
	std::string url = baseUrlV2 + "/preferred/matrixLive/queryZlmLiveDevices/" + std::to_string(liveAccountId) + "?token=" + token;

	//std::string url = "http://39.98.176.148:8083/preferred/matrixLive/queryZlmLiveDevices/1";	

	auto task = new HttpRequestTask(
		[this](std::string url, std::string token) -> std::string {
			cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyPeer{false});
			cpr::Url cprUrl{url};
			cpr::Header headers{{"token", token}};
			auto r = cpr::GetAsync(cprUrl, headers, sslOpts).get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			auto r = json::parse(response);
			if ((!r["result"].is_null()) && (r["result"].is_array())) {
				std::list<GBSLiveDevices> devices =
					GBSLiveDevices::fromJsonArray(r["result"].dump());
				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onListDevices(devices, -1);
				}
			} else {
				GBSLiveDevices device = GBSLiveDevices::fromJson(r["result"].dump());
				std::list<GBSLiveDevices> devices;
				devices.push_back(device);
				std::lock_guard<std::mutex> guard(cs);
				for (auto it = handlers.begin(); it != handlers.end(); it++) {
					(*it)->onListDevices(devices, -1);
				}
			}
			
		},
		url, token);
	task->run();
}

void GBSHttpClient::startLiveTranscribeV2(int duration, int streamLogId) {
	executor->addTask(std::bind(&GBSHttpClient::startLiveTranscribeTaskV2, this, duration, streamLogId));
}

void GBSHttpClient::startLiveTranscribeTaskV2(int duration, int streamLogId) {
	json body = {{"duration", duration}, {"streamLogId", streamLogId}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/startLiveTranscribe";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::endLiveTranscribeV2(int streamLogId) {
	executor->addTask(std::bind(&GBSHttpClient::endLiveTranscribeTaskV2, this, streamLogId));

    }
void GBSHttpClient::endLiveTranscribeTaskV2(int streamLogId) {
	json body = {{"streamLogId", streamLogId}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/endLiveTranscribe";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::getLiveTranscribeStatusV2(int streamLogId) {
	executor->addTask(std::bind(&GBSHttpClient::getLiveTranscribeStatusTaskV2, this, streamLogId));

    }
void GBSHttpClient::getLiveTranscribeStatusTaskV2(int streamLogId) {
	json body = {{"streamLogId", streamLogId}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/getLiveTranscribeStatus";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::queryLiveTranscribeByStreamLogIdV2(int streamLogId) {
	executor->addTask(std::bind(&GBSHttpClient::queryLiveTranscribeByStreamLogIdTaskV2, this, streamLogId));

}
void GBSHttpClient::queryLiveTranscribeByStreamLogIdTaskV2(int streamLogId) {
	std::string url = baseUrlV2 + "/preferred/matrixLive/queryLiveTranscribeByStreamLogId/" + std::to_string(streamLogId);
	auto task = new HttpRequestTask(
		[this](std::string url, std::string token) -> std::string {
			cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyPeer{false});
			cpr::Header headers = {{"token", token}};
			cpr::Url cprUrl{url};
			auto r = cpr::GetAsync(cprUrl, headers, sslOpts).get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, token);
	task->run();
}

void GBSHttpClient::sendTranscribeLiveHeartBeatV2(int id) {
	executor->addTask(std::bind(&GBSHttpClient::sendTranscribeLiveHeartBeatTaskV2, this, id));
}
void GBSHttpClient::sendTranscribeLiveHeartBeatTaskV2(int id) {
	json body = {{"id", id}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/sendTranscribeLiveHeartBeat";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}

void GBSHttpClient::startTranscribeLiveV2(int liveAccountId, std::list<std::string> liveDevices, int streamLogId) {
	executor->addTask(
		std::bind(&GBSHttpClient::startTranscribeLiveTaskV2, this, liveAccountId, liveDevices, streamLogId));
}
void GBSHttpClient::startTranscribeLiveTaskV2(int liveAccountId, std::list<std::string> liveDevices, int streamLogId) {
	std::string liveDevicesString = "";
	for (auto liveDevice : liveDevices) {
		liveDevicesString += liveDevice;
		liveDevicesString += ",";
	}

	json body = {{"liveAccountId", liveAccountId},
		     {"liveDevices", liveDevicesString},
		     {"streamLogId", streamLogId}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/startTranscribeLive";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);

	task->run();
}


void GBSHttpClient::endTranscribeLiveV2(int id) {
	executor->addTask(
		std::bind(&GBSHttpClient::endTranscribeLiveTaskV2, this, id));
}
void GBSHttpClient::endTranscribeLiveTaskV2(int id) {
	json body = {{"id", id}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/endTranscribeLive";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}


void GBSHttpClient::pageZlmStreamLogV2(int streamLogId, int liveServerId, int pageNum, int pageSize)
{
	executor->addTask(
		std::bind(&GBSHttpClient::pageZlmStreamLogTaskV2, this, streamLogId, liveServerId, pageNum, pageSize));
}
void GBSHttpClient::pageZlmStreamLogTaskV2(int streamLogId, int liveServerId, int pageNum, int pageSize)
{
	json body = {{"liveAccountId", streamLogId},
		     {"liveServerId", liveServerId},
		     {"pageNum", pageNum},
		     {"pageSize", pageSize}};
	std::string url = baseUrlV2 + "/preferred/matrixLive/pageZlmStreamLog";
	auto task = new HttpRequestTask(
		[this](std::string url, std::string body, std::string token) -> std::string {
			auto r = cpr::PostAsync(cpr::Url{url}, cpr::Body{body},
						cpr::Header{{"Content-Type", "application/json"},
							    {"host", httpHostV2},
							    {"token", token}},
						cpr::VerifySsl{false})
					 .get();
			if (r.status_code == 200) {
				return r.text;
			} else {
				std::cerr << "[ERROR] send websocket"
					  << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					  << std::endl;
				QLogE("SendWebsocket: status code:%d body %s", r.status_code, r.text);

				return "Error";
			}
		},
		[this](const std::string &response) {
			if (response.compare("Error")) {
			}
		},
		url, body.dump(), token);
	task->run();
}
