#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gin {

class Context;
class Router;

using Handler = std::function<void(Context&)>;
using Handlers = std::vector<Handler>;

using Middleware = std::function<void(Context&)>;

struct RouteInfo {
    std::string path;
    std::string method;
    Handler handler;
    std::vector<Middleware> middlewares;
};

class RouterGroup {
public:
    RouterGroup(const std::string& prefix, Router* router)
        : prefix_(prefix), router_(router) {}

    void Get(const std::string& path, Handler handler);
    void Post(const std::string& path, Handler handler);
    void Put(const std::string& path, Handler handler);
    void Delete(const std::string& path, Handler handler);
    void Patch(const std::string& path, Handler handler);
    void Options(const std::string& path, Handler handler);
    void Head(const std::string& path, Handler handler);
    void Any(const std::string& path, Handler handler);
    void Use(Middleware middleware);

private:
    std::string prefix_;
    Router* router_;
    std::vector<Middleware> middlewares_;
};

}  // namespace gin
