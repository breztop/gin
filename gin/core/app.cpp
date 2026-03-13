#include "core/app.hpp"

#include "http/server.hpp"
#include "utils/logger.hpp"

namespace gin {

App App::Default() {
    App app;
    app.router_ = std::make_unique<Router>();
    return app;
}

void App::Use(Middleware middleware) {
    global_middlewares_.push_back(std::move(middleware));
    middleware_chain_.Use(std::move(middleware));
}

RouterGroup App::Group(const std::string& prefix) {
    return RouterGroup(prefix, router_.get());
}

void App::Get(const std::string& path, Handler handler) {
    router_->AddRoute("GET", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: GET {}", path);
}

void App::Post(const std::string& path, Handler handler) {
    router_->AddRoute("POST", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: POST {}", path);
}

void App::Put(const std::string& path, Handler handler) {
    router_->AddRoute("PUT", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: PUT {}", path);
}

void App::Delete(const std::string& path, Handler handler) {
    router_->AddRoute("DELETE", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: DELETE {}", path);
}

void App::Patch(const std::string& path, Handler handler) {
    router_->AddRoute("PATCH", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: PATCH {}", path);
}

void App::Options(const std::string& path, Handler handler) {
    router_->AddRoute("OPTIONS", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: OPTIONS {}", path);
}

void App::Head(const std::string& path, Handler handler) {
    router_->AddRoute("HEAD", path, std::move(handler), global_middlewares_);
    LOG_DEBUG("Route registered: HEAD {}", path);
}

static const std::vector<std::string> kMethods = {"GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS", "HEAD"};

void App::Any(const std::string& path, Handler handler) {
    for (const auto& method : kMethods) {
        router_->AddRoute(method, path, handler, global_middlewares_);
    }
    LOG_DEBUG("Route registered: ANY {}", path);
}

void App::Static(const std::string& prefix, const std::string& root) {
    static_prefix_ = prefix;
    static_root_ = root;
    LOG_INFO("Static files: {} -> {}", prefix, root);
}

void App::StaticFS(const std::string& prefix, const std::string& root) {
    static_prefix_ = prefix;
    static_root_ = root;
    LOG_INFO("Static files (FS): {} -> {}", prefix, root);
}

void App::Run(int port) {
    Logger::Instance().Init("gin", "debug");
    LOG_INFO("Gin server starting on port {}", port);

    server_ = std::make_unique<Server>("0.0.0.0", port, router_.get());
    server_->Run();
}

void App::Stop() {
    if (server_) {
        server_->Stop();
    }
}

}  // namespace gin
