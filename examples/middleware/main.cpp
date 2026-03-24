// Middleware Example
// 展示常用中间件的使用

#include <iostream>

#include "core/engine.hpp"
#include "middleware/cors.hpp"
#include "middleware/logger.hpp"
#include "middleware/recovery.hpp"
#include "middleware/request_id.hpp"

int main() {
    auto engine = gin::Engine::Default();

    // 使用内置中间件
    engine->Use(gin::Logger());
    engine->Use(gin::Recovery());
    engine->Use(gin::middleware::RequestID());

    // 自定义中间件 - 记录请求时间
    engine->Use([](gin::Context::Shared ctx) {
        ctx->Set("request_start", std::string("started"));
        ctx->Next();
    });

    // 公开路由
    engine->Get("/", [](gin::Context::Shared ctx) {
        auto request_id = ctx->GetString("request_id");
        ctx->JSON(200, {{"message", "Welcome to Gin++"}, {"request_id", request_id}});
    });

    engine->Get("/ping", [](gin::Context::Shared ctx) {
        ctx->JSON(200, {{"message", "pong"}});
    });

    // 使用 CORS 中间件的路由组
    auto api = engine->Group("/api");
    api->Use(gin::CORS());
    {
        api->Get("/data", [](gin::Context::Shared ctx) {
            ctx->JSON(200, {{"data", {"item1", "item2", "item3"}}});
        });

        api->Post("/data", [](gin::Context::Shared ctx) {
            nlohmann::json body;
            if (ctx->ShouldBindJSON(body)) {
                ctx->JSON(201, {{"message", "Created"}, {"data", body}});
            } else {
                ctx->JSON(400, {{"error", "Invalid JSON"}});
            }
        });
    }

    // 测试 panic 恢复
    engine->Get("/panic", [](gin::Context::Shared) {
        throw std::runtime_error("This is a test panic!");
    });

    // 测试自定义错误处理
    engine->Get("/error", [](gin::Context::Shared ctx) {
        ctx->JSON(500, {{"error", "Something went wrong"}});
    });

    std::cout << "Middleware example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Routes:" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/ping" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/data" << std::endl;
    std::cout << "  curl -X POST -H 'Content-Type: application/json' -d '{\"key\":\"value\"}' "
                 "http://127.0.0.1:8080/api/data"
              << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/panic  # Test recovery middleware" << std::endl;
    engine->Run(8080);

    return 0;
}
