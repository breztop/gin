#pragma once

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <vector>

#include "http/connection_manager.hpp"

namespace gin {


class Router;
using boost::asio::ip::tcp;

class Server {
public:
    Server(const std::string& address, int port, Router* router);
    ~Server();

    void Run();
    void Stop();

private:
    void Accept();
    void StartWorkerThreads();
    void JoinThreads();

    boost::asio::io_context ioc_;
    tcp::acceptor acceptor_;
    Router* router_;
    ConnectionManager connection_manager_;
    std::vector<std::thread> workers_;
    bool running_ = false;
};

}  // namespace gin
