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
	QLogD("Http client start");
	do {
		size_t pos = baseUrl.find("https://");
		if (pos != std::string::npos) {
			httpHost = baseUrl.substr(
				pos + 8); // 加7是因为"http://"有7个字符
			break;
		}

		pos = baseUrl.find("http://");
		if (pos != std::string::npos) {
			httpHost = baseUrl.substr(
				pos + 7); // 加7是因为"http://"有7个字符
			break;
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


void GBSHttpClient::sendWebsocketMsg(std::string msg) {
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
								rtmpPushUrl);
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

void gbsLoginWithCheckVersion(std::string phone, std::string password,
			      int loginType, std::string vendor)
{
	GBSHttpClient::getInstance()->loginWithCheckVersion(
		phone, password, loginType, vendor);
}


std::string GBSHttpClient::getEequipmentNo() {
	std::lock_guard<std::mutex> guard(cs);
	return equipmentInfo.getEquipmentNo();
}
