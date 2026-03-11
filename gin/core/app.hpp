#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/context.hpp"
#include "core/middleware_chain.hpp"
#include "core/router.hpp"
#include "core/types.hpp"
#include "http/server.hpp"

namespace gin {

class App {
public:
    static App Default();

    void Use(Middleware middleware);
    RouterGroup Group(const std::string& prefix);

    void Get(const std::string& path, Handler handler);
    void Post(const std::string& path, Handler handler);
    void Put(const std::string& path, Handler handler);
    void Delete(const std::string& path, Handler handler);
    void Patch(const std::string& path, Handler handler);
    void Options(const std::string& path, Handler handler);
    void Head(const std::string& path, Handler handler);

    void Static(const std::string& prefix, const std::string& root);

    void Run(int port);

private:
    App() = default;

    std::unique_ptr<Router> router_;
    std::vector<Middleware> global_middlewares_;
    MiddlewareChain middleware_chain_;
    std::string static_root_;
    std::string static_prefix_;
    std::unique_ptr<Server> server_;
};

inline void RouterGroup::Get(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("GET", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Post(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("POST", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Use(Middleware middleware) { middlewares_.push_back(std::move(middleware)); }

}  // namespace gin
