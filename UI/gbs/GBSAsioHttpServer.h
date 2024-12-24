#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <mutex>




namespace asio_http {


class Server {
public:
    static std::shared_ptr<Server> Create();
    virtual ~Server() {};
    virtual void start() = 0;
    virtual void stop() = 0;

};



} // namespace asio_http



using GBSAsioHttpServer = asio_http::Server;
