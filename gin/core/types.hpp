#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gin {

class Context;
class Router;

using Handler = std::function<void(std::shared_ptr<Context>)>;
using Handlers = std::vector<Handler>;

inline Handlers MakeHandlers(Handler h) {
    Handlers chain;
    chain.push_back(std::move(h));
    return chain;
}

template <typename... Args>
inline Handlers MakeHandlers(Handler h, Args&&... args) {
    Handlers chain;
    chain.push_back(std::move(h));
    auto rest = MakeHandlers(std::forward<Args>(args)...);
    chain.insert(chain.end(), rest.begin(), rest.end());
    return chain;
}

using Middleware = std::function<void(std::shared_ptr<Context>)>;

struct RouteInfo {
    std::string path;
    std::string method;
    Handler handler;
    std::vector<Middleware> middlewares;
};

class RouterGroup {
public:
    using Shared = std::shared_ptr<RouterGroup>;
    RouterGroup(const std::string& prefix, Router* router) : prefix_(prefix), router_(router) {}

    void Get(const std::string& path, Handler handler);
    void Post(const std::string& path, Handler handler);
    void Put(const std::string& path, Handler handler);
    void Delete(const std::string& path, Handler handler);
    void Patch(const std::string& path, Handler handler);
    void Options(const std::string& path, Handler handler);
    void Head(const std::string& path, Handler handler);
    void Any(const std::string& path, Handler handler);
    void Handle(const std::string& method, const std::string& path,
                std::initializer_list<Handler> handlers);
    void Use(Middleware middleware);

    const std::string& BasePath() const { return prefix_; }

private:
    std::string prefix_;
    Router* router_;
    std::vector<Middleware> middlewares_;
};

}  // namespace gin
