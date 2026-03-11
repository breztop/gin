#include "http/connection_manager.hpp"

#include "http/session.hpp"
#include "utils/logger.hpp"

namespace gin {

// using boost::asio::ip::tcp;

void ConnectionManager::Start(std::shared_ptr<HTTPSession> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.insert(session);
    LOG_DEBUG("Connection started, total connections: {}", connections_.size());
}

void ConnectionManager::Stop(std::shared_ptr<HTTPSession> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(session);
    LOG_DEBUG("Connection stopped, remaining connections: {}", connections_.size());
}

void ConnectionManager::StopAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    LOG_INFO("Stopping all connections, count: {}", connections_.size());
    for (auto& connection : connections_) {
        connection->Stop();
    }
    connections_.clear();
    LOG_INFO("All connections stopped");
}

}  // namespace gin
