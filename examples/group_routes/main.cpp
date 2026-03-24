// Route Grouping Example
// 基于 gin-gonic/examples/group-routes 改编

#include <iostream>

#include "core/engine.hpp"

int main() {
    auto engine = gin::Engine::Default();

    // 公开路由
    engine->Get("/", [](gin::Context::Shared ctx) {
        ctx->JSON(200, {{"message", "Welcome to the API"}});
    });

    engine->Get("/health", [](gin::Context::Shared ctx) {
        ctx->JSON(200, {{"status", "ok"}});
    });

    // API v1 路由组
    auto apiV1 = engine->Group("/api/v1");
    {
        apiV1->Get("/users", [](gin::Context::Shared ctx) {
            ctx->JSON(200, {{"version", "v1"}, {"users", nlohmann::json::array({"alice", "bob"})}});
        });

        apiV1->Get("/users/:id", [](gin::Context::Shared ctx) {
            auto id = ctx->Param("id");
            ctx->JSON(200, {{"version", "v1"}, {"user_id", id}});
        });

        apiV1->Post("/users", [](gin::Context::Shared ctx) {
            nlohmann::json body;
            if (ctx->ShouldBindJSON(body)) {
                ctx->JSON(201, {{"message", "User created"}, {"user", body}});
            } else {
                ctx->JSON(400, {{"error", "Invalid JSON"}});
            }
        });
    }

    // API v2 路由组
    auto apiV2 = engine->Group("/api/v2");
    {
        apiV2->Get("/users", [](gin::Context::Shared ctx) {
            ctx->JSON(200, {{"version", "v2"},
                            {"users", nlohmann::json::array({"alice", "bob", "charlie"})},
                            {"total", 3}});
        });

        apiV2->Get("/users/:id", [](gin::Context::Shared ctx) {
            auto id = ctx->Param("id");
            ctx->JSON(200, {{"version", "v2"},
                            {"user_id", id},
                            {"details",
                             {{"name", "User " + id}, {"email", "user" + id + "@example.com"}}}});
        });
    }

    // Admin 路由组
    auto admin = engine->Group("/admin");
    {
        admin->Use([](gin::Context::Shared ctx) {
            auto token = ctx->request.GetHeader("X-Admin-Token");
            if (token != "secret123") {
                ctx->AbortWithStatusJSON(403, {{"error", "Admin access required"}});
                return;
            }
            ctx->Next();
        });

        admin->Get("/stats", [](gin::Context::Shared ctx) {
            ctx->JSON(200, {{"users", 100}, {"requests", 12345}});
        });

        admin->Post("/config", [](gin::Context::Shared ctx) {
            ctx->JSON(200, {{"message", "Config updated"}});
        });
    }

    std::cout << "Route grouping example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Public routes:" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/health" << std::endl;
    std::cout << "API v1 routes:" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/v1/users" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/v1/users/123" << std::endl;
    std::cout << "API v2 routes:" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/v2/users" << std::endl;
    std::cout << "Admin routes (need token):" << std::endl;
    std::cout << "  curl -H 'X-Admin-Token: secret123' http://127.0.0.1:8080/admin/stats"
              << std::endl;
    engine->Run(8080);

    return 0;
}
