#include "http/server.hpp"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "http/session.hpp"
#include "utils/logger.hpp"


namespace gin {


class Router;


namespace beast = boost::beast;
namespace http = beast::http;

Server::Server(const std::string& address, int port, Router* router)
    : acceptor_(ioc_)
    , router_(router) {
    boost::asio::ip::tcp::resolver resolver(ioc_);
    auto endpoint = resolver.resolve(address, std::to_string(port)).begin()->endpoint();

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    LOG_INFO("Server listening on {}:{}", address, port);
}

Server::~Server() { Stop(); }

void Server::Run() {
    running_ = true;
    auto count = std::thread::hardware_concurrency();
    if (count == 0) count = 2;
    LOG_INFO("Starting {} worker threads", count);
    Accept();
    StartWorkerThreads();

    for (auto& t : workers_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void Server::Stop() {
    if (!running_) return;

    running_ = false;
    connection_manager_.StopAll();
    ioc_.stop();
    JoinThreads();

    LOG_INFO("Server stopped");
}

void Server::Accept() {
    acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            LOG_DEBUG("Accepted new connection from {}", socket.remote_endpoint().address().to_string());
            auto session =
                std::make_shared<HTTPSession>(std::move(socket), router_, connection_manager_);
            session->Start();
        } else {
            LOG_ERROR("Accept error: {}", ec.message());
        }
        Accept();
    });
}

void Server::StartWorkerThreads() {
    auto count = std::thread::hardware_concurrency();
    if (count == 0) count = 2;

    for (unsigned i = 0; i < count; ++i) {
        workers_.emplace_back([this]() {
            ioc_.run();
        });
    }
}

void Server::JoinThreads() {
    for (auto& t : workers_) {
        if (t.joinable()) {
            t.join();
        }
    }
    workers_.clear();
}

}  // namespace gin
