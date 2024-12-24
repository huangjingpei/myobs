#include <websocketpp/config/asio_client.hpp>

#include <websocketpp/client.hpp>

#include <iostream>
#include <chrono>

#include "WebSocketClient.h"
#include <QDebug>
#include "gbs/common/QBizLogger.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<asio::ssl::context> context_ptr;
typedef client::connection_ptr connection_ptr;

class WebSocketClientImpl : public WebSocketClient {
public:
	typedef WebSocketClientImpl type;
	typedef std::chrono::duration<int, std::micro> dur_type;

	WebSocketClientImpl()
	{
		m_handle = {};
		m_endpoint.set_access_channels(websocketpp::log::alevel::all);
		m_endpoint.set_error_channels(websocketpp::log::elevel::all);

		// Initialize ASIO
		m_endpoint.init_asio();

		// Register our handlers
		m_endpoint.set_socket_init_handler(
			bind(&type::on_socket_init, this, ::_1));
		m_endpoint.set_tls_init_handler(
			bind(&type::on_tls_init, this, ::_1));
		m_endpoint.set_message_handler(
			bind(&type::on_message, this, ::_1, ::_2));
		m_endpoint.set_open_handler(bind(&type::on_open, this, ::_1));
		m_endpoint.set_close_handler(bind(&type::on_close, this, ::_1));
		m_endpoint.set_fail_handler(bind(&type::on_fail, this, ::_1));
		m_endpoint.set_ping_handler(bind(&type::on_ping, this, ::_1, ::_2));
		m_endpoint.set_pong_handler(bind(&type::on_pong, this, ::_1, ::_2));
		m_endpoint.set_pong_timeout_handler(bind(&type::on_pong_timeout, this, ::_1, ::_2));
		m_endpoint.start_perpetual();


	}
	~WebSocketClientImpl() override { Stop();
	}

	void setName(std::string name) override { m_socket_name = name;
	}

private:
	void start(std::string uri)
	{
		m_url = uri;
		m_running = true;
		websocketpp::lib::error_code ec;
		client::connection_ptr con = m_endpoint.get_connection(uri, ec);
		if (ec) {
			m_endpoint.get_alog().write(
				websocketpp::log::alevel::app, ec.message());
			return;
		}
		//con->set_proxy("http://humupdates.uchicago.edu:8443");
		//con->replace_header("Sec-WebSocket-Protocol", "protoo");
		m_endpoint.connect(con);
		m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);



	}


	void on_socket_init(websocketpp::connection_hdl hdl)
	{
		m_socket_init = std::chrono::high_resolution_clock::now();
		m_handle = hdl;
	}

	context_ptr on_tls_init(websocketpp::connection_hdl)
	{
		m_tls_init = std::chrono::high_resolution_clock::now();
		context_ptr ctx =
			websocketpp::lib::make_shared<asio::ssl::context>(
				asio::ssl::context::sslv23);

		try {
			ctx->set_options(
				asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3);
		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
		ctx->set_verify_mode(
			asio::ssl::verify_none); // Enabling verification of server certificate
		ctx->set_default_verify_paths(); // Use default paths for verification
		return ctx;
	}

	void on_fail(websocketpp::connection_hdl hdl)
	{
		client::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);

		std::cout << "Fail handler" << std::endl;
		std::cout << con->get_state() << std::endl;
		std::cout << con->get_local_close_code() << std::endl;
		std::cout << con->get_local_close_reason() << std::endl;
		std::cout << con->get_remote_close_code() << std::endl;
		std::cout << con->get_remote_close_reason() << std::endl;
		std::cout << con->get_ec() << " - " << con->get_ec().message()
			  << std::endl;

		qDebug() << con->get_state();
		qDebug() << con->get_local_close_code();
		qDebug() << con->get_local_close_reason();
					
		qDebug() << con->get_remote_close_code();
		qDebug() << con->get_remote_close_reason();
					
		//qDebug() << con->get_ec();
		qDebug() << con->get_ec().message();

		QLogD("on faled [%#x] tag:%s reason %s， url %s", this, m_socket_name.c_str(), con->get_ec().message().c_str(), m_url.c_str());

			
	}

	bool on_ping(websocketpp::connection_hdl, std::string msg) {
		return true;
	}
	void on_pong(websocketpp::connection_hdl, std::string msg) {

	}

	void on_pong_timeout(websocketpp::connection_hdl, std::string) {

	}

	void on_open(websocketpp::connection_hdl)
	{
		QLogD("on open [%#x] tag:%s", this, m_socket_name.c_str());

		m_open = std::chrono::high_resolution_clock::now();
		std::lock_guard<std::mutex> lock(listenerMutex);
		//m_endpoint.send(hdl, "", websocketpp::frame::opcode::text);
		for (auto it = listeners.begin(); it != listeners.end(); it++) {
			(*it)->onOpen();
		}
	}
	void on_message(websocketpp::connection_hdl, message_ptr msg)
	{
		std::lock_guard<std::mutex> lock(listenerMutex);	
		m_message = std::chrono::high_resolution_clock::now();
		for (auto it = listeners.begin(); it != listeners.end(); it++) {
			(*it)->onMessage(msg->get_payload());
		}
	}
	void on_close(websocketpp::connection_hdl)
	{
		QLogD("on close [%#x] tag:%s", this, m_socket_name.c_str());
		m_close = std::chrono::high_resolution_clock::now();

		std::cout << "Socket Init: "
			  << std::chrono::duration_cast<dur_type>(
				     m_socket_init - m_start)
				     .count()
			  << std::endl;
		std::cout << "TLS Init: "
			  << std::chrono::duration_cast<dur_type>(m_tls_init -
								  m_start)
				     .count()
			  << std::endl;
		std::cout << "Open: "
			  << std::chrono::duration_cast<dur_type>(m_open -
								  m_start)
				     .count()
			  << std::endl;
		std::cout << "Message: "
			  << std::chrono::duration_cast<dur_type>(m_message -
								  m_start)
				     .count()
			  << std::endl;
		std::cout << "Close: "
			  << std::chrono::duration_cast<dur_type>(m_close -
								  m_start)
				     .count()
			  << std::endl;
		if (!m_exited) {
			std::thread(&WebSocketClientImpl::retry_connection,
				    this)
				.detach();
		} else {
			for (auto it = listeners.begin(); it != listeners.end();
			     it++) {
				(*it)->onClose();
			}
		}

	}
	void send_mseeage(std::string &msg)
	{
		m_endpoint.send(m_handle, msg,
				websocketpp::frame::opcode::text);
	}

	void retry_connection()
	{
		++m_retry_times;
		QLogD("retry connect %#x retry times %d", this, m_retry_times);
		while ((m_should_reconnect) && (!m_exited)){
			m_running = false;
			m_endpoint.stop_perpetual();

	
			// Ensure the WebSocket connection is closed
			if (m_handle.lock()) {
				try {
					m_endpoint.close(m_handle, websocketpp::close::status::going_away, "");
				} catch (const websocketpp::exception &e) {
					std::cerr << "WebSocket close error: " << e.what() << std::endl;
				}
			}

			// Wait for the `run_client` thread to exit
			if (m_thread && m_thread->joinable()) {
				m_thread->join();
			}

			std::cout << "Attempting to reconnect in " << m_reconnect_interval << " seconds..."
				  << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(m_reconnect_interval));
			start(m_url); // 重新尝试连接
		}
	}




public:
	void Start(std::string url) override
	{
		DWORD threadId = GetCurrentThreadId();
		qDebug() << "start thread id" << threadId << " url " <<  url;
		QLogD("Start thread id %d", threadId);
		start(url);
	}
	void Stop() override
	{
		m_endpoint.stop_perpetual();
		DWORD threadId = GetCurrentThreadId();
		qDebug() << "stop thread id" << threadId;
		QLogD("Stop thread id %d", threadId);

		m_exited = true;

		m_running = false;

		// Ensure the WebSocket connection is closed
		if (m_handle.lock()) {
			try {
				m_endpoint.close(m_handle, websocketpp::close::status::going_away, "");
			} catch (const websocketpp::exception &e) {
				std::cerr << "WebSocket close error: " << e.what() << std::endl;
			}
		}

		// Wait for the `run_client` thread to exit
		if (m_thread && m_thread->joinable()) {
			m_thread->join();
		}

	}

	void Send(std::string msg) override { send_mseeage(msg); }

	void SetReconnect(bool reconnect = true) override
	{
		m_should_reconnect = reconnect;
	}

	bool IsRunnig() override {
		return m_running;
	}

private:
	client m_endpoint;
	websocketpp::connection_hdl m_handle;
	std::chrono::high_resolution_clock::time_point m_start;
	std::chrono::high_resolution_clock::time_point m_socket_init;
	std::chrono::high_resolution_clock::time_point m_tls_init;
	std::chrono::high_resolution_clock::time_point m_open;
	std::chrono::high_resolution_clock::time_point m_message;
	std::chrono::high_resolution_clock::time_point m_close;
	std::atomic<bool> m_should_reconnect{true}; // 是否应该重连
	std::atomic<bool> m_exited{false};
	std::atomic<bool> m_running{false};
	int m_reconnect_interval{5};
	int m_retry_times{0};
	std::string m_url;
	
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

	std::mutex stopMutex;
	std::condition_variable stopCondition;
	std::string m_socket_name;
};

WebSocketClient::WebSocketClient() {}
WebSocketClient::~WebSocketClient() {
}

std::shared_ptr<WebSocketClient> WebSocketClient::Create()
{
	return std::make_shared<WebSocketClientImpl>();
}



void WebSocketClient::RegisterHandler(WssEventListener* handler) {
	std::lock_guard<std::mutex> lock(listenerMutex);
	listeners.push_back(handler);
}
void WebSocketClient::UnRegisterHandler(WssEventListener* handler) {
	std::lock_guard<std::mutex> lock(listenerMutex);
	listeners.remove_if(
		[handler](WssEventListener *item) {
		return item == handler;
	});
}

