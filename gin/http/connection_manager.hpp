#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>

namespace gin {


class HTTPSession;

class ConnectionManager {
public:
    ConnectionManager() = default;

    void Start(std::shared_ptr<HTTPSession> session);
    void Stop(std::shared_ptr<HTTPSession> session);
    void StopAll();

private:
    std::unordered_set<std::shared_ptr<HTTPSession>> connections_;
    std::mutex mutex_;
};

}  // namespace gin
