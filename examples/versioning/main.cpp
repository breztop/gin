// API Versioning Example
// 基于 gin-gonic/examples/versioning 改编

#include <iostream>

#include "core/engine.hpp"

// 认证中间件
gin::Middleware AuthMiddleware() {
    return [](gin::Context& ctx) {
        auto user = ctx.PostForm("user");
        auto password = ctx.PostForm("password");

        if (user == "foo" && password == "bar") {
            ctx.Next();
            return;
        }
        ctx.AbortWithStatusJSON(401, {{"error", "Unauthorized"}});
    };
}

int main() {
    auto engine = gin::Engine::Default();

    // API V1
    auto apiV1 = engine.Group("/v1");

    apiV1.Get("/users", [](gin::Context& ctx) {
        ctx.JSON(200, {{"version", "v1"}, {"users", {"alice", "bob"}}});
    });

    // V1 受保护接口
    auto authV1 = engine.Group("/v1");
    authV1.Use(AuthMiddleware());

    authV1.Post("/users/add", [](gin::Context& ctx) {
        ctx.JSON(200, {{"message", "V1 User added"}});
    });

    // API V2
    auto apiV2 = engine.Group("/v2");

    apiV2.Get("/users", [](gin::Context& ctx) {
        ctx.JSON(200, {{"version", "v2"}, {"users", {"alice", "bob", "charlie"}}});
    });

    // V2 受保护接口
    auto authV2 = engine.Group("/v2");
    authV2.Use(AuthMiddleware());

    authV2.Post("/users/add", [](gin::Context& ctx) {
        ctx.JSON(200, {{"message", "V2 User added"}});
    });

    std::cout << "Versioning example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Try: curl http://127.0.0.1:8080/v1/users" << std::endl;
    std::cout << "Try: curl http://127.0.0.1:8080/v2/users" << std::endl;
    std::cout << "Try: curl -X POST -d 'user=foo&password=bar' http://127.0.0.1:8080/v1/users/add"
              << std::endl;
    engine.Run(8080);

    return 0;
}
