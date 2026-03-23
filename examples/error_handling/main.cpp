// Error Handling Example
// 演示错误处理的各种方式

#include <iostream>

#include "core/engine.hpp"
#include "middleware/recovery.hpp"

// 自定义错误类型
struct AppError {
    int code;
    std::string message;
    std::string details;
};

int main() {
    auto engine = gin::Engine::Default();

    // 使用 Recovery 中间件捕获异常
    engine.Use(gin::Recovery());

    // 自定义 NoRoute 处理
    engine.NoRoute([](gin::Context& ctx) {
        ctx.JSON(404, {{"error", "Not Found"},
                       {"path", ctx.request.path},
                       {"message", "The requested resource does not exist"}});
    });

    // 基本错误响应
    engine.Get("/error/basic", [](gin::Context& ctx) {
        ctx.JSON(400, {{"error", "Bad Request"}});
    });

    // 使用 Error 方法
    engine.Get("/error/simple", [](gin::Context& ctx) {
        ctx.Error(500, "Internal Server Error");
    });

    // 使用 AbortWithError
    engine.Get("/error/abort", [](gin::Context& ctx) {
        ctx.AbortWithError(403, "Forbidden: Access denied");
    });

    // 使用 AbortWithStatusJSON
    engine.Get("/error/json", [](gin::Context& ctx) {
        ctx.AbortWithStatusJSON(
            422, {{"error", "Validation Failed"},
                  {"details", {{"field", "email"}, {"message", "Invalid email format"}}}});
    });

    // 模拟异常
    engine.Get("/error/throw", [](gin::Context& ctx) {
        throw std::runtime_error("Something went wrong!");
    });

    // 验证错误示例
    engine.Post("/validate", [](gin::Context& ctx) {
        nlohmann::json body;
        if (!ctx.ShouldBindJSON(body)) {
            ctx.AbortWithStatusJSON(400, {{"error", "Invalid JSON"}});
            return;
        }

        // 验证必填字段
        std::vector<std::string> errors;
        if (!body.contains("name") || body["name"].get<std::string>().empty()) {
            errors.push_back("name is required");
        }
        if (!body.contains("email") || body["email"].get<std::string>().empty()) {
            errors.push_back("email is required");
        }
        if (!body.contains("age") || !body["age"].is_number()) {
            errors.push_back("age must be a number");
        }

        if (!errors.empty()) {
            ctx.AbortWithStatusJSON(422, {{"error", "Validation Failed"}, {"details", errors}});
            return;
        }

        ctx.JSON(200, {{"message", "Valid"}, {"data", body}});
    });

    // 带状态码的错误
    engine.Get("/error/:code", [](gin::Context& ctx) {
        int code = std::stoi(ctx.Param("code"));
        std::string message;

        switch (code) {
            case 400:
                message = "Bad Request";
                break;
            case 401:
                message = "Unauthorized";
                break;
            case 403:
                message = "Forbidden";
                break;
            case 404:
                message = "Not Found";
                break;
            case 500:
                message = "Internal Server Error";
                break;
            default:
                message = "Unknown Error";
                break;
        }

        ctx.JSON(code, {{"error", message}, {"code", code}});
    });

    // 成功示例
    engine.Get("/success", [](gin::Context& ctx) {
        ctx.JSON(200, {{"message", "Success!"}});
    });

    std::cout << "Error handling example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Try these endpoints:" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/success" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/error/basic" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/error/simple" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/error/404" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/not-exist  # Custom 404" << std::endl;
    std::cout
        << R"(  curl -X POST -H 'Content-Type: application/json' -d '{"name":"test"}' http://127.0.0.1:8080/validate)"
        << std::endl;
    engine.Run(8080);

    return 0;
}
