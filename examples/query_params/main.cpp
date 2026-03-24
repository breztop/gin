// Query Parameters Example
// 演示查询参数的各种用法

#include <iostream>

#include "core/engine.hpp"

int main() {
    auto engine = gin::Engine::Default();

    // 基本查询参数
    engine->Get("/search", [](gin::Context::Shared ctx) {
        auto q = ctx->Query("q");
        auto page = ctx->DefaultQuery("page", "1");
        auto limit = ctx->DefaultQuery("limit", "10");

        ctx->JSON(200, {{"query", q}, {"page", page}, {"limit", limit}});
    });

    // 多值查询参数
    engine->Get("/filter", [](gin::Context::Shared ctx) {
        auto tags = ctx->GetQueryArray("tag");

        ctx->JSON(200, {{"tags", tags}, {"count", tags.size()}});
    });

    // 路径参数
    engine->Get("/users/:id", [](gin::Context::Shared ctx) {
        auto id = ctx->Param("id");
        ctx->JSON(200, {{"user_id", id}});
    });

    // 多个路径参数
    engine->Get("/users/:user_id/posts/:post_id", [](gin::Context::Shared ctx) {
        auto user_id = ctx->Param("user_id");
        auto post_id = ctx->Param("post_id");

        ctx->JSON(200, {{"user_id", user_id}, {"post_id", post_id}});
    });

    // 通配符路径
    engine->Get("/files/*path", [](gin::Context::Shared ctx) {
        auto path = ctx->Param("path");
        ctx->JSON(200, {{"file_path", path}});
    });

    // 组合使用
    engine->Get("/api/:version/users", [](gin::Context::Shared ctx) {
        auto version = ctx->Param("version");
        auto sort = ctx->DefaultQuery("sort", "name");
        auto order = ctx->DefaultQuery("order", "asc");

        ctx->JSON(200, {{"version", version}, {"sort", sort}, {"order", order}});
    });

    std::cout << "Query parameters example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Try:" << std::endl;
    std::cout << "  curl 'http://127.0.0.1:8080/search?q=hello&page=2'" << std::endl;
    std::cout << "  curl 'http://127.0.0.1:8080/filter?tag=cpp&tag=web&tag=api'" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/users/123" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/users/1/posts/42" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/files/path/to/file.txt" << std::endl;
    std::cout << "  curl 'http://127.0.0.1:8080/api/v2/users?sort=email&order=desc'" << std::endl;
    engine->Run(8080);

    return 0;
}
