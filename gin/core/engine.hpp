#pragma once

#include <initializer_list>
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

class Engine {
public:
    static Engine Default();

    void Use(Middleware middleware);
    RouterGroup Group(const std::string& prefix);

    void Handle(const std::string& method, const std::string& path,
                std::initializer_list<Handler> handlers);
    void Match(const std::vector<std::string>& methods, const std::string& path, Handler handler);

    void Get(const std::string& path, Handler handler);
    void Get(const std::string& path, Middleware middleware, Handler handler);
    void Post(const std::string& path, Handler handler);
    void Post(const std::string& path, Middleware middleware, Handler handler);
    void Put(const std::string& path, Handler handler);
    void Put(const std::string& path, Middleware middleware, Handler handler);
    void Delete(const std::string& path, Handler handler);
    void Delete(const std::string& path, Middleware middleware, Handler handler);
    void Patch(const std::string& path, Handler handler);
    void Patch(const std::string& path, Middleware middleware, Handler handler);
    void Options(const std::string& path, Handler handler);
    void Options(const std::string& path, Middleware middleware, Handler handler);
    void Head(const std::string& path, Handler handler);
    void Head(const std::string& path, Middleware middleware, Handler handler);
    void Any(const std::string& path, Handler handler);
    void Any(const std::string& path, Middleware middleware, Handler handler);

    void NoRoute(Handler handler);
    void NoMethod(Handler handler);

    void Static(const std::string& prefix, const std::string& root);
    void StaticFS(const std::string& prefix, const std::string& root);

    void SetMode(const std::string& mode);

    std::vector<RouteInfo> RoutesInfo();

    void Run(int port);
    void Stop();

private:
    Engine() = default;

    std::unique_ptr<Router> router_;
    std::vector<Middleware> global_middlewares_;
    MiddlewareChain middleware_chain_;
    std::string static_root_;
    std::string static_prefix_;
    std::unique_ptr<Server> server_;
    Handler no_route_handler_;
    Handler no_method_handler_;
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

inline void RouterGroup::Use(Middleware middleware) {
    middlewares_.push_back(std::move(middleware));
}

inline void RouterGroup::Put(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("PUT", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Delete(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("DELETE", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Patch(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("PATCH", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Options(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("OPTIONS", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Head(const std::string& path, Handler handler) {
    if (router_) {
        router_->AddRoute("HEAD", prefix_ + path, std::move(handler), middlewares_);
    }
}

inline void RouterGroup::Any(const std::string& path, Handler handler) {
    if (router_) {
        static const std::vector<std::string> methods = {"GET",   "POST",    "PUT", "DELETE",
                                                         "PATCH", "OPTIONS", "HEAD"};
        for (const auto& method : methods) {
            router_->AddRoute(method, prefix_ + path, handler, middlewares_);
        }
    }
}

inline void RouterGroup::Handle(const std::string& method, const std::string& path,
                                std::initializer_list<Handler> handlers) {
    if (router_ && handlers.size() > 0) {
        auto it = handlers.begin();
        Handler final_handler = *it;
        ++it;
        for (; it != handlers.end(); ++it) {
            Handler next = *it;
            Handler prev = std::move(final_handler);
            final_handler = [prev = std::move(prev), next = std::move(next)](Context& ctx) {
                prev(ctx);
                if (!ctx.IsAborted()) {
                    next(ctx);
                }
            };
        }
        router_->AddRoute(method, prefix_ + path, std::move(final_handler), middlewares_);
    }
}

}  // namespace gin
