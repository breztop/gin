#include "core/engine.hpp"

#include "http/server.hpp"
#include "utils/logger.hpp"

namespace gin {

Engine Engine::Default() {
    Engine engine;
    engine.router_ = std::make_unique<Router>();
    return engine;
}

void Engine::Use(Middleware middleware) {
    global_middlewares_.push_back(std::move(middleware));
    middleware_chain_.Use(std::move(middleware));
}

RouterGroup Engine::Group(const std::string& prefix) { return RouterGroup(prefix, router_.get()); }

void Engine::Handle(const std::string& method, const std::string& path,
                    std::initializer_list<Handler> handlers) {
    if (handlers.size() == 0) return;

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
    router_->AddRoute(method, path, std::move(final_handler), global_middlewares_);
    LOG_DEBUG("Route registered: {} {} ({} handlers)", method, path, handlers.size());
}

void Engine::Match(const std::vector<std::string>& methods, const std::string& path,
                   Handler handler) {
    for (const auto& method : methods) {
        router_->AddRoute(method, path, handler, global_middlewares_);
    }
    LOG_DEBUG("Route registered: MATCH {} methods for {}", methods.size(), path);
}

void Engine::Get(const std::string& path, Handler handler) {
    router_->AddRoute("GET", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: GET {}", path);
}

void Engine::Get(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("GET", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: GET {} (with middleware)", path);
}

void Engine::Post(const std::string& path, Handler handler) {
    router_->AddRoute("POST", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: POST {}", path);
}

void Engine::Post(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("POST", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: POST {} (with middleware)", path);
}

void Engine::Put(const std::string& path, Handler handler) {
    router_->AddRoute("PUT", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: PUT {}", path);
}

void Engine::Put(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("PUT", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: PUT {} (with middleware)", path);
}

void Engine::Delete(const std::string& path, Handler handler) {
    router_->AddRoute("DELETE", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: DELETE {}", path);
}

void Engine::Delete(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("DELETE", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: DELETE {} (with middleware)", path);
}

void Engine::Patch(const std::string& path, Handler handler) {
    router_->AddRoute("PATCH", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: PATCH {}", path);
}

void Engine::Patch(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("PATCH", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: PATCH {} (with middleware)", path);
}

void Engine::Options(const std::string& path, Handler handler) {
    router_->AddRoute("OPTIONS", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: OPTIONS {}", path);
}

void Engine::Options(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("OPTIONS", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: OPTIONS {} (with middleware)", path);
}

void Engine::Head(const std::string& path, Handler handler) {
    router_->AddRoute("HEAD", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: HEAD {}", path);
}

void Engine::Head(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    router_->AddRoute("HEAD", path, std::move(handler), middlewares);
    LOG_DEBUG("Route registered: HEAD {} (with middleware)", path);
}

static const std::vector<std::string> kMethods = {"GET",   "POST",    "PUT", "DELETE",
                                                  "PATCH", "OPTIONS", "HEAD"};

void Engine::Any(const std::string& path, Handler handler) {
    for (const auto& method : kMethods) {
        router_->AddRoute(method, path, handler, global_middlewares_);
    }
    LOG_DEBUG("Route registered: ANY {}", path);
}

void Engine::Any(const std::string& path, Middleware middleware, Handler handler) {
    auto middlewares = global_middlewares_;
    middlewares.push_back(std::move(middleware));
    for (const auto& method : kMethods) {
        router_->AddRoute(method, path, handler, middlewares);
    }
    LOG_DEBUG("Route registered: ANY {} (with middleware)", path);
}

void Engine::NoRoute(Handler handler) {
    no_route_handler_ = std::move(handler);
    router_->SetNoRouteHandler(no_route_handler_);
    LOG_DEBUG("NoRoute handler registered");
}

void Engine::NoMethod(Handler handler) {
    no_method_handler_ = std::move(handler);
    router_->SetNoMethodHandler(no_method_handler_);
    LOG_DEBUG("NoMethod handler registered");
}

void Engine::Static(const std::string& prefix, const std::string& root) {
    static_prefix_ = prefix;
    static_root_ = root;
    LOG_INFO("Static files: {} -> {}", prefix, root);
}

void Engine::StaticFS(const std::string& prefix, const std::string& root) {
    static_prefix_ = prefix;
    static_root_ = root;
    LOG_INFO("Static files (FS): {} -> {}", prefix, root);
}

void Engine::SetMode(const std::string& mode) {
    Logger::Instance().SetLevel(mode == "release" ? "warn" : "debug");
    LOG_INFO("Mode set to: {}", mode);
}

std::vector<RouteInfo> Engine::RoutesInfo() { return router_->RoutesInfo(); }

void Engine::Run(int port) {
    Logger::Instance().Init("gin", "debug");
    LOG_INFO("Gin server starting on port {}", port);

    server_ = std::make_unique<Server>("0.0.0.0", port, router_.get());
    server_->Run();
}

void Engine::Stop() {
    if (server_) {
        server_->Stop();
    }
}

}  // namespace gin
