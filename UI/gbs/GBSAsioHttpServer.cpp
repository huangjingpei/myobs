#include "GBSAsioHttpServer.h"
#include <asio.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

using namespace asio_http;

// 将图标数据改为 std::vector
const std::vector<unsigned char> GetFaviconData() {
    static const std::vector<unsigned char> icon_data = {
        0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x00, 0x00, 0x01, 0x00, 0x20,
        0x00, 0x28, 0x04, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF
    };
    return icon_data;
}

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(asio::io_service &service) : socket_(service) {}

    asio::ip::tcp::socket &socket() { return socket_; }

    void start();
    void stop();

private:
    void readRequestLine();
    void readHeaderLines();
    void handleRequest();
    void readBody(std::size_t content_length);

    void handlePost(const std::string &body);
    void sendResponse(const std::string &status, const std::string &body);
    std::string processRequest();
    void parseRequestLine(const std::string &line);
    void parseHeaderLine(const std::string &line);
    int getContentLength() const;

    asio::ip::tcp::socket socket_;
    std::shared_ptr<std::vector<char>> body_buf_;
    asio::streambuf buffer_;
    std::string method_;
    std::string url_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    bool stopped_ = false;
};

class ServerImpl : public Server {
public:
    ServerImpl(int port);
    void start() override;
    void stop() override;

private:
    void accept();
    void handle_accept(std::shared_ptr<Session> new_session, const asio::error_code &error);

    asio::io_service io_service_;
    asio::io_service::strand strand_;
    asio::ip::tcp::acceptor acceptor_;
    int port_;
    std::set<std::shared_ptr<Session>> sessions_;
    std::mutex mutex_;
    std::thread server_thread_;
};

void Session::start() {
    readRequestLine();
}

void Session::stop() {
    if (socket_.is_open()) {
        asio::error_code ec;
        socket_.cancel(ec);  // 先取消所有未完成的异步操作
        socket_.close(ec);   // 然后关闭 socket
    }
    stopped_ = true;
    std::cout << "Session stopped." << std::endl;
}

void Session::readRequestLine() {
    auto self = shared_from_this();
    asio::async_read_until(socket_, buffer_, "\r\n", 
        [self](const asio::error_code &ec, std::size_t bytes_transferred) {
            if (ec) {
                self->sendResponse("400 Bad Request", "Failed to read request line.");
                return;
            }

            try {
                std::istream stream(&self->buffer_);
                std::string line;
                // 读取到 \r
                std::getline(stream, line, '\r');
                // 跳过 \n
                stream.get();

                // 处理请求行
                self->parseRequestLine(line);
                // 继续读取头部
                self->readHeaderLines();
            } catch (const std::exception& e) {
                std::cerr << "Error reading request line: " << e.what() << std::endl;
                self->sendResponse("400 Bad Request", "Error parsing request line");
            }
    });
}

void Session::readHeaderLines() {
    auto self = shared_from_this();
    asio::async_read_until(socket_, buffer_, "\r\n\r\n", 
        [self](const asio::error_code &ec, std::size_t bytes_transferred) {
            if (ec) {
                self->sendResponse("400 Bad Request", "Failed to read header lines.");
                return;
            }

            try {
                std::istream stream(&self->buffer_);
                std::string line;
                // 读取所有头部行直到遇到空行
                while (std::getline(stream, line, '\r')) {
                    // 跳过紧随其后的 \n
                    stream.get();
                    
                    // 空行标志着头部结束
                    if (line.empty()) {
                        self->handleRequest();
                        break;
                    }
                    
                    // 处理非空的头部行
                    if (!line.empty()) {
                        self->parseHeaderLine(line);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error reading headers: " << e.what() << std::endl;
                self->sendResponse("400 Bad Request", "Error parsing headers");
            }
    });
}

void Session::parseRequestLine(const std::string &line) {
    try {
        std::istringstream ss(line);
        if (!(ss >> method_ >> url_ >> version_)) {
            sendResponse("400 Bad Request", "Invalid request line format");
            return;
        }
        std::cout << "Request for resource: " << url_ << std::endl;
    } catch (const std::exception) {
        sendResponse("400 Bad Request", "Error parsing request line");
    }
}

void Session::parseHeaderLine(const std::string &line) {
    try {
        auto delimiter_pos = line.find(':');
        if (delimiter_pos != std::string::npos && delimiter_pos > 0) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            
            // 去除前后空格
            while (!value.empty() && std::isspace(value.front())) {
                value.erase(0, 1);
            }
            while (!value.empty() && std::isspace(value.back())) {
                value.pop_back();
            }
            
            headers_[key] = value;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error parsing header line: " << e.what() << std::endl;
    }
}

int Session::getContentLength() const {
    auto it = headers_.find("content-length");
    if (it != headers_.end()) {
        return std::stoi(it->second);
    }
    return 0;
}

void Session::handleRequest() {
    try {
        auto self = shared_from_this();
        std::string response = processRequest();
	asio::async_write(socket_, asio::buffer(response.c_str(), response.size()),
            [self](const asio::error_code &ec, std::size_t) {
                if (!ec) {
                    self->socket_.close();
                }
            });
    } catch (const std::exception &e) {
        sendResponse("500 Internal Server Error", "Error processing request");
        std::cerr << "Error in handleRequest: " << e.what() << std::endl;
    }
}

std::string Session::processRequest() {
    std::ostringstream response;

    if (url_ == "/favicon.ico") {
        const auto icon_data = GetFaviconData();
        
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: image/x-icon\r\n";  // 更标准的 MIME 类型
        response << "Content-Length: " << icon_data.size() << "\r\n";
        response << "Cache-Control: public, max-age=31536000\r\n\r\n";  // 添加缓存控制
        
        response.write(reinterpret_cast<const char*>(icon_data.data()), icon_data.size());
    } else if (url_ == "/") {
        std::string html =
            "<html><body><h1>Hello, World!</h1><p>This is a test web server in C++.</p></body></html>";
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << html.size() << "\r\n\r\n";
        response << html;
    } else if (url_ == "/biz/danmaku/live/msg") {
        auto it = headers_.find("Content-Length");
        if (it != headers_.end()) {
            std::size_t content_length = std::stoul(it->second);
            readBody(content_length);
	    return "";
        } else {
            response << "HTTP/1.1 400 Bad Request\r\n";
            response << "Content-Length: 0\r\n\r\n";
        }
    } else {
        std::string html =
            "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
        response << "HTTP/1.1 404 Not Found\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << html.size() << "\r\n\r\n";
        response << html;
    }

    return std::move(response.str());
}
void Session::readBody(std::size_t content_length)
{
	auto self = shared_from_this();
	auto buffer = std::make_shared<std::vector<char>>(content_length); // 引用计数为1
	size_t size = buffer->size();
	asio::async_read(socket_, asio::buffer(buffer->data(), buffer->size()),
			 [self, buffer, content_length](const asio::error_code &ec, std::size_t bytes_transferred) {


				 if (ec) {
					 std::cerr << "Error reading body: " << ec.message() << std::endl;
					 self->sendResponse("400 Bad Request", "Failed to read request body.");
					 return; // lambda结束后，buffer 引用计数减1
				 }

				 if (bytes_transferred != content_length) {
					 std::cerr << "Content length mismatch: expected " << content_length << ", got "
						   << bytes_transferred << std::endl;
					 self->sendResponse("400 Bad Request", "Content length mismatch.");
					 return; // lambda结束后，buffer 引用计数减1
				 }

				 // 处理读取的请求体数据
				 std::string body(buffer->begin(), buffer->end());
				 self->handlePost(body); // lambda结束后，buffer 引用计数减1
			 });
}

void Session::handlePost(const std::string &body) {
    std::cout << "Received POST body: " << body << std::endl;
    sendResponse("200 OK", "Request received successfully.");
}

void Session::sendResponse(const std::string &status, const std::string &body) {
    auto self = shared_from_this();
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Content-Type: text/plain\r\n";
    response << "\r\n";
    response << body;

    asio::async_write(socket_, asio::buffer(response.str()), [self](const asio::error_code &ec, std::size_t) {
        if (!ec) {
            self->socket_.close();
        }
    });
}

ServerImpl::ServerImpl(int port)
    : io_service_(), strand_(io_service_), acceptor_(io_service_), port_(port) {
    asio::ip::address ip_address = asio::ip::address::from_string("127.0.0.1"); 
    //asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port_);
    asio::ip::tcp::endpoint endpoint(ip_address, port_);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
}

void ServerImpl::start() {
    server_thread_ = std::thread([this]() {
        accept();
        io_service_.run();
    });
}

void ServerImpl::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        io_service_.stop();
        for (auto &session : sessions_) {
            session->stop();
        }
        sessions_.clear();
    }

    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}

void ServerImpl::accept() {
    auto new_session = std::make_shared<Session>(io_service_);
    acceptor_.async_accept(new_session->socket(), strand_.wrap([this, new_session](const asio::error_code &error) {
        handle_accept(new_session, error);
    }));
}

void ServerImpl::handle_accept(std::shared_ptr<Session> new_session, const asio::error_code &error) {
    if (!error) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.insert(new_session);
        new_session->start();
        accept();
    }
}

std::shared_ptr<Server> Server::Create() {
    return std::make_shared<ServerImpl>(7979);
}
