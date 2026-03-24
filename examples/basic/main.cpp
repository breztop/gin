#include <iostream>
#include <map>
#include <string>

#include "core/auth.hpp"
#include "core/engine.hpp"
#include "gin/http/type.hpp"
#include "nlohmann/json.hpp"


static std::map<std::string, std::string> db;

auto SetupRoutes() -> std::unique_ptr<gin::Engine> {
    auto r = gin::Engine::Default();

    // Ping
    r->Get("/ping", [](gin::Context::Shared ctx) {
        ctx->String(gin::HttpStatus::ok, "pong");
    });

    // Get user value
    r->Get("/user/:name", [](gin::Context::Shared ctx) {
        std::string user = ctx->Param("name");
        auto it = db.find(user);
        if (it != db.end()) {
            ctx->JSON(gin::HttpStatus::ok, nlohmann::json{{"user", user}, {"value", it->second}});
        } else {
            ctx->JSON(gin::HttpStatus::ok, nlohmann::json{{"user", user}, {"status", "no value"}});
        }
    });

    auto authorized = r->Group("/", gin::BasicAuth(gin::Accounts{
                                        {"foo", "bar"},
                                        {"manu", "123"},
                                    }));

    authorized->Post("/admin", [](gin::Context::Shared ctx) {
        std::string user = std::any_cast<std::string>(ctx->MustGet(gin::AuthUserKey));

        auto body = ctx->GetRawData();
        try {
            auto j = nlohmann::json::parse(body);
            if (j.contains("value") && j["value"].is_string()) {
                db[user] = j["value"].get<std::string>();
                ctx->JSON(gin::HttpStatus::ok, nlohmann::json{{"status", "ok"}});
                return;
            }
        } catch (...) {
        }
        ctx->JSON(gin::HttpStatus::bad_request, nlohmann::json{{"error", "invalid payload"}});
    });

    return r;
}

int main() {
    auto r = SetupRoutes();

    std::cout << "try: \n"
              << "curl http://localhost:8080/ping \n"
              << "curl http://localhost:8080/user/foo \n"
              << "curl -u foo:bar -X POST http://localhost:8080/admin -d '{\"value\": \"hello "
                 "world\"}' \n"
              << "curl http://localhost:8080/user/foo \n"
              << "curl http://localhost:8080/user/nonexistent \n";

    r->Run(8080);


    // test
    // curl -u foo:bar -X POST http://localhost:8080/admin -d '{"value": "hello world"}'
    // --> {"status":"ok"}
    // curl http://localhost:8080/user/foo  --> {"user":"foo","value":"hello world"}
    // curl http://localhost:8080/ping  --> pong
    // curl http://localhost:8080/user/nonexistent --> {"user":"nonexistent","status":"no value"}

    return 0;
}
