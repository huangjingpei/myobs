#include "GBSHttpsServer.h"
extern "C" {
#include "mongoose.h"
}
// Self signed certificates, see
// https://github.com/cesanta/mongoose/blob/master/test/certs/generate.sh
#ifdef TLS_TWOWAY
static const char *s_tls_ca =
	"-----BEGIN CERTIFICATE-----\n"
	"MIIBFTCBvAIJAMNTFtpfcq8NMAoGCCqGSM49BAMCMBMxETAPBgNVBAMMCE1vbmdv\n"
	"b3NlMB4XDTI0MDUwNzE0MzczNloXDTM0MDUwNTE0MzczNlowEzERMA8GA1UEAwwI\n"
	"TW9uZ29vc2UwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASuP+86T/rOWnGpEVhl\n"
	"fxYZ+pjMbCmDZ+vdnP0rjoxudwRMRQCv5slRlDK7Lxue761sdvqxWr0Ma6TFGTNg\n"
	"epsRMAoGCCqGSM49BAMCA0gAMEUCIQCwb2CxuAKm51s81S6BIoy1IcandXSohnqs\n"
	"us64BAA7QgIgGGtUrpkgFSS0oPBlCUG6YPHFVw42vTfpTC0ySwAS0M4=\n"
	"-----END CERTIFICATE-----\n";
#endif
static const char *s_tls_cert = R"(-----BEGIN CERTIFICATE-----
MIIEBTCCAu2gAwIBAgIUC8/bMrdPeAovDwGE1US+W6bD6S0wDQYJKoZIhvcNAQEL
BQAwgZExCzAJBgNVBAYTAkNOMREwDwYDVQQIDAhaaGVKaWFuZzERMA8GA1UEBwwI
SGFuZ1pob3UxEzARBgNVBAoMCmdyYWRkdS5jb20xDzANBgNVBAsMBmdyYWRkdTEP
MA0GA1UEAwwGR1JBRERVMSUwIwYJKoZIhvcNAQkBFhZodWFuZ2ppbmdwZWlAZ21h
aWwuY29tMB4XDTI0MTAyMTEwMjc0MFoXDTM0MTAxOTEwMjc0MFowgZExCzAJBgNV
BAYTAkNOMREwDwYDVQQIDAhaaGVKaWFuZzERMA8GA1UEBwwISGFuZ1pob3UxEzAR
BgNVBAoMCmdyYWRkdS5jb20xDzANBgNVBAsMBmdyYWRkdTEPMA0GA1UEAwwGR1JB
RERVMSUwIwYJKoZIhvcNAQkBFhZodWFuZ2ppbmdwZWlAZ21haWwuY29tMIIBIjAN
BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAukhlHkPTHk2KsZnSF8sOgMoLkPcM
PElS8hXrwwhVmTAynPU0K03mt69fqvpNxIvM3Uz4uQuD4wxw9VDHUsYtxmjUOGal
m5qOTej4ZEzac/h/7IrGnIXo5ASGOL4XxCLG6qeotRLOwppRR0MPYgX5AZ7wFJek
yC3oLm0yl+PvkTDq0bNi5QvBVkEeIQcKwEvVEgFfzp5K3MsfROPizDosWb2l0kdY
f+BQSqvrcNiC2WOojqwQeOl8hqve6BdB5VqBYxY/to5nmQIEEG8SA2Mi9CWoCjY9
29/ZRNTTIDfeBwh9wkGDOhIdW9HN+vFIRL9T6VZLa2uQfRVMpgewhTzxYQIDAQAB
o1MwUTAdBgNVHQ4EFgQUDhTKCiAo0DdVCHtfhVS+spBY/FkwHwYDVR0jBBgwFoAU
DhTKCiAo0DdVCHtfhVS+spBY/FkwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0B
AQsFAAOCAQEAU6uf8lOTjpanPV0/Ws+qgoWbFM4TMKztSYjTUmqTE7dXyGySZ91X
Q/hmK1GRlQfU+R549aZ9E3gJ0cn0hRyc9iI0e1VMo5MCKZ52FVgKjXB9gp+/IQsL
S/Pu0j/yGTAxWn/nviQHJnxcqz1aMmgz0vlKwT4hmjpPfCSlo0DwcTYfERBS4u+J
UC+w93UIr1Zxep6EStrWIn6f1OST1Sm+Zj002eqvAsR7PLsGCentqOkHFVis2wSo
5HOzyT52aQ71o6tGZeJxB5w3wMeYYV78SPBlbo1us6yXKkuguDq+ujDW96ySzRm5
SQwNzgf2Q1tK/YP+pVgxPZUNC3oJEEpmtA==
-----END CERTIFICATE-----
)";

static const char *s_tls_key = R"(-----BEGIN PRIVATE KEY-----
MIIEuwIBADANBgkqhkiG9w0BAQEFAASCBKUwggShAgEAAoIBAQC6SGUeQ9MeTYqx
mdIXyw6AyguQ9ww8SVLyFevDCFWZMDKc9TQrTea3r1+q+k3Ei8zdTPi5C4PjDHD1
UMdSxi3GaNQ4ZqWbmo5N6PhkTNpz+H/sisachejkBIY4vhfEIsbqp6i1Es7CmlFH
Qw9iBfkBnvAUl6TILegubTKX4++RMOrRs2LlC8FWQR4hBwrAS9USAV/Onkrcyx9E
4+LMOixZvaXSR1h/4FBKq+tw2ILZY6iOrBB46XyGq97oF0HlWoFjFj+2jmeZAgQQ
bxIDYyL0JagKNj3b39lE1NMgN94HCH3CQYM6Eh1b0c368UhEv1PpVktra5B9FUym
B7CFPPFhAgMBAAECgf8uUBIUdKWnP2MQYia7/rJB4jO6bywkA4SEtbRXxnTRSlfO
Sa08xkk4xveneDLos3lzOuw55Ws11mmBkSlHJ/XLv1Uhwmj8PM4CnOIaE0ijo++E
8OjnG4DZtrbFGL1qnlQi6nOdsAZyJeUPMMH9vTRSGQ2ja/QJ+BAkZw3WS35JRzUM
WdUht9i2aKgau9fZV+48qxPe7yKeefG19uhv67+7g5YJGooIBVAT2yCxrsmwFsCv
2JoB6fkphTtqgYQbin20Rhmvd98Tw0F3dVLTR1K+TMnhB8hXTp8cDqk0g4sEvzg0
VWXrOerEPku+ZwEeI0olhPoQoHSUQXpRFz9laRcCgYEA5d5wXB/Z1KmYpjMNrQTd
qARr37MzjtJ9GZq16r30IxYaYMgymVangIjmsrigo5Efwt6T49XGlpR1ooYTJaM1
tgvtnFtngj3/fC8x/2/igQ16A0oKfD5Yv+KwWXr0dEbkR5LgmxG7U7K6mApucs2P
0017trFAreKIzQabhx1SgDsCgYEAz3WHbUYqhSRMJvQDdoqzTNVeafezPhs76TLS
yuVGGuyRCaJFxB72TkhBc/IJHWcruSMiGdKD6fbP0nCrMeqpyVDL+G+TlX6apbS3
HuqhS7BNa5ZP5yZqdAx8QFpPtFNxgqmqY8O7mROb2syPUwJO44bS32rl0legqv5P
O0VmdxMCgYBnssJjfObq/htkFrH7tK8+BciYYSJIMwLGkzmwDUNetLDvqkDRVfs4
jAnu5EVrpKN8aKa06JKvCYhL6kom94radmUZ3U5Dvi5MWoomntXCvE6nvzYduI6W
+e/uXf57miOPel4i9ovYhU+SGVqcJcnVXCYMU8hG6WQngeyVto+qUwKBgQCZT0JA
R5z+fT9lfefdP0yXWAqIhstH9zpD8BIEbIBtPytDkk66SyM74Hu2IkQIHYkbLNJz
gLWJPaKA4lAw+Dh2lCXXtyS6UjAYir7UsxF1bJkaC5+Ks0Hx9pQPvm2uVgfJRnar
eSb2TZ5ch9TWxBrmW2kUP9wMoEMUehC1PWkmTwKBgDfoSgpsFPw2elJe+tPs3BUH
RlFvN73WYvNMP0tcH73u8SY0lpC5JtepvrH6WHAu/KK06yH1sQM2ou4N09YfHSfi
t/zmYMuvTxwezC8JzD4krtM3CIsJcGJLRhBE+hgZ62v0vJoWtNxh2XPQYhxZOu9K
QWeH+tVM2g0vee09bFoB
-----END PRIVATE KEY-----)";

#include <list>
#include <mutex>
#include "nlohmann/json.hpp"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSMainCollector.h"
// static std::mutex gMutex;
// static std::list<std::shared_ptr<DammaMSG>> glbDanmaMsg;

// void processDanmaItem(const nlohmann::json jsonObject) {
// 	const std::lock_guard<std::mutex> lock(gMutex);
// 	std::string type = jsonObject["type"].get<std::string>();
// 	if (type == "MemberMessage") {
// 		auto danma = std::make_shared<DammaMemberMSG>();
// 		danma->type = "MemberMessage";
// 		danma->name = jsonObject["name"];
// 		danma->head_image = jsonObject["head_image"];
// 		danma->content = jsonObject["content"];
// 		danma->msgType = 1;
// 		glbDanmaMsg.push_back(danma);

// 	} else if (type == "ChatMessage") {
// 		auto danma = std::make_shared<DanmaChatMessage>();
// 		danma->type = "ChatMessage";
// 		danma->name = jsonObject["name"];
// 		danma->head_image = jsonObject["head_image"];
// 		danma->content = jsonObject["content"];
// 		danma->msgType = 2;

// 		glbDanmaMsg.push_back(danma);
// 	} else if (type == "GiftMessage") {
// 		auto danma = std::make_shared<DanmaGiftMessage>();
// 		danma->type = "GiftMessage";
// 		danma->name = jsonObject["name"];
// 		danma->head_image = jsonObject["head_image"];
// 		danma->content = jsonObject["content"];
// 		danma->gift_name = jsonObject["gift_name"];
// 		danma->gift_count = jsonObject["gift_count"];
// 		danma->msgType = 3;
// 		glbDanmaMsg.push_back(danma);
// 	} else if (type == "SocialMessage") {
// 		auto danma = std::make_shared<DanmaSocialMessage>();
// 		danma->type = "SocialMessage";
// 		danma->name = jsonObject["name"];
// 		danma->head_image = jsonObject["head_image"];
// 		danma->content = jsonObject["content"];
// 		danma->msgType = 4;
// 		glbDanmaMsg.push_back(danma);
// 	} else if (type == "LikeMessage") {
// 		auto danma = std::make_shared<DanmaLikeMessage>();
// 		danma->type = "LikeMessage";
// 		danma->name = jsonObject["name"];
// 		danma->head_image = jsonObject["head_image"];
// 		danma->content = jsonObject["content"];
// 		danma->count = jsonObject["count"];
// 		danma->msgType = 5;
// 		glbDanmaMsg.push_back(danma);
// 	}

// }


// const std::list<std::shared_ptr<DammaMSG>>& getDanmuList()
// {

// 	std::lock_guard<std::mutex> lock(gMutex);
// 	return glbDanmaMsg;
	 

// }


void processDanmaItem(nlohmann::json &jsonObject) {
	std::string liveId = GBSMainCollector::getInstance()->getDanmaKuName();
	std::string liveDeviceId = GBSMainCollector::getInstance()->getLiveDeviceId();
	std::string deviceName = GBSMainCollector::getInstance()->getDeviceName();
	std::string platform = GBSMainCollector::getInstance()->getDanmakuPlat();
	std::string platformAccount = GBSMainCollector::getInstance()->getLivePlatAcct();
	jsonObject["liveId"] = liveId;
	jsonObject["liveDeviceId"] = liveDeviceId;
	jsonObject["deviceName"] = deviceName;
	jsonObject["platform"] = platform;
	jsonObject["platformAcct"] = platformAccount;
}
void processRecvMessage(const char* mesage) {
	
	auto jsonObject = nlohmann::json::parse(mesage);
	if (jsonObject.is_object()) {
		processDanmaItem(jsonObject);
	} else if (jsonObject.is_array()) {
		int size = (int)jsonObject.size();
		for (int i = 0; i < size; i++) {
			processDanmaItem(jsonObject[i]);
		}
	}
	std::string newMsg =jsonObject.dump();
	GBSHttpClient::getInstance()->sendWebsocketMsgV2(newMsg);
}
static void cb(struct mg_connection *c, int ev, void *ev_data)
{
	if (ev == MG_EV_ACCEPT && c->fn_data != NULL) {
		struct mg_tls_opts opts;
		memset(&opts, 0, sizeof(opts));
#ifdef TLS_TWOWAY
		opts.ca = mg_str(s_tls_ca);
#endif
		opts.cert = mg_str(s_tls_cert);
		opts.key = mg_str(s_tls_key);
		mg_tls_init(c, &opts);
	}
	if (ev == MG_EV_HTTP_MSG) {
		struct mg_http_message *hm = (struct mg_http_message *)ev_data;

		if (mg_match(hm->uri, mg_str("/biz/danmaku/live/msg"), NULL)) {
			processRecvMessage(hm->body.buf);
			mg_http_reply(c, 200,
				      "Content-Type: application/json\r\n",
				      "{%m:%g}\n", mg_print_esc, 0, "result",
				      0);
		} else if (mg_match(hm->uri, mg_str("/test"), NULL)) {
			mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%s:%s}\n", 
				      "result", "OK");
			return;
		}
		//// Log request
		//MG_INFO(("%.*s %.*s %lu -> %.*s %lu", hm->method.len,
		//	 hm->method.buf, hm->uri.len, hm->uri.buf, hm->body.len,
		//	 3, c->send.buf + 9, c->send.len));
	}
}

void *GBSHttpsInit(std::string httpAddr, std::string httpsAddr,
		   std::string cert, std::string key)
{
	GBSHttpsContext *context = new GBSHttpsContext;
	context->debugLevel = MG_LL_INFO;
	context->httpAddr = httpAddr.empty() ? "" : httpAddr;
	context->httpsAddr = httpsAddr.empty() ? "" : httpsAddr;
	context->cert = cert.empty() ? "" : cert;
	context->key = key.empty() ? "" : key;
	return context;
}

int GBSHttpsRun(void *handle)
{
	int ret = -1;
	GBSHttpsHandle *httpsHandle = (GBSHttpsHandle *)handle;
	do {
		if (!httpsHandle) {
			goto ending;
		}

		mg_log_set(httpsHandle->debugLevel);
		mg_mgr_init(&httpsHandle->mgr);
		if (!httpsHandle->httpAddr.empty()) {
			if ((httpsHandle->c = mg_http_listen(
				     &httpsHandle->mgr,
				     httpsHandle->httpAddr.c_str(), cb,
				     NULL)) == NULL) {
				ret = -2;
				goto ending;
			}
		}

		if (!httpsHandle->httpsAddr.empty()) {
			if ((httpsHandle->c = mg_http_listen(
				     &httpsHandle->mgr,
				     httpsHandle->httpsAddr.c_str(), cb,
				     NULL)) == NULL) {
				ret = -3;
				goto ending;
			}
		}
		httpsHandle->running = true;
		httpsHandle->serverThread =
			std::make_shared<std::thread>([httpsHandle]() {
				while (httpsHandle->running) {
					mg_mgr_poll(&(httpsHandle->mgr), 1000);
				}
				httpsHandle->stopCV.notify_all();
			});

	} while (0);
	ret = 0;
ending:
	return ret;
}

void GBSHttpsDestroy(void *handle)
{
	GBSHttpsHandle *httpsHandle = (GBSHttpsHandle *)handle;
	if (handle != nullptr) {
		httpsHandle->running = false;
		if (httpsHandle->serverThread && httpsHandle->serverThread->joinable()) {
			httpsHandle->serverThread->join(); // 等待 serverThread 结束
		}
		mg_mgr_free(&httpsHandle->mgr);
		delete httpsHandle;
		httpsHandle = nullptr;
	}
}
