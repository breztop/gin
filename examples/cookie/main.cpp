#include <print>

#include "core/cookie.hpp"
#include "core/engine.hpp"

gin::Middleware CookieAuth() {
    return [](gin::Context::Shared ctx) {
        auto cookie = ctx->GetCookie("label");
        if (cookie == "ok") {
            ctx->Next();
            return;
        }
        ctx->AbortWithStatusJSON(403, {{"error", "Forbidden: no valid cookie"}});
    };
}

int main() {
    auto engine = gin::Engine::Default();
    engine->SetMode("debug");

    // 登录接口 - 设置 cookie
    engine->Get("/login", [](gin::Context::Shared ctx) {
        gin::Cookie cookie;
        cookie.name = "label";
        cookie.value = "ok";
        cookie.max_age = 30;
        cookie.path = "/";
        cookie.http_only = true;
        ctx->SetCookie(cookie);
        ctx->String(200, "Login success!");
    });

    // 受保护的接口 - 需要 cookie 认证
    engine->Get("/home", CookieAuth(), [](gin::Context::Shared ctx) {
        ctx->JSON(200, {{"data", "Your home page"}});
    });

    // 受保护的用户信息接口
    engine->Get("/profile", CookieAuth(), [](gin::Context::Shared ctx) {
        ctx->JSON(200, {{"user", "admin"}, {"email", "admin@example.com"}});
    });

    using std::println;
    println("Cookie example starting on http://127.0.0.1:8080, Try: ");
    println("curl http://127.0.0.1:8080/home");  // {"error":"Forbidden: no valid cookie"}
    println("curl -c cookies.txt http://127.0.0.1:8080/login");  // Login success!
    println("curl -b cookies.txt http://127.0.0.1:8080/home");   // {"data":"Your home page"}

    println("curl -b cookies.txt http://127.0.0.1:8080/profile");
    // {"user":"admin","email":"admin@example.com"}


    engine->Run(8080);

    return 0;
}
