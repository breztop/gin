// JSON API Example
// 演示 JSON 请求和响应的各种用法

#include <iostream>

#include "core/engine.hpp"

struct Product {
    int id;
    std::string name;
    double price;
    std::vector<std::string> tags;
};

void to_json(nlohmann::json& j, const Product& p) {
    j = nlohmann::json{{"id", p.id}, {"name", p.name}, {"price", p.price}, {"tags", p.tags}};
}

void from_json(const nlohmann::json& j, Product& p) {
    if (j.contains("name")) j.at("name").get_to(p.name);
    if (j.contains("price")) j.at("price").get_to(p.price);
    if (j.contains("tags")) j.at("tags").get_to(p.tags);
}

int main() {
    auto engine = gin::Engine::Default();

    // 简单 JSON 响应
    engine.Get("/api/status", [](gin::Context& ctx) {
        ctx.JSON(200, {{"status", "ok"}, {"version", "1.0.0"}, {"uptime", 12345}});
    });

    // 格式化 JSON 响应
    engine.Get("/api/status/pretty", [](gin::Context& ctx) {
        nlohmann::json data = {{"status", "ok"},
                               {"version", "1.0.0"},
                               {"features", {"json", "routing", "middleware"}}};
        ctx.IndentedJSON(200, data);
    });

    // 接收并返回 JSON
    engine.Post("/api/echo", [](gin::Context& ctx) {
        nlohmann::json body;
        if (ctx.ShouldBindJSON(body)) {
            ctx.JSON(200, {{"echo", body}});
        } else {
            ctx.JSON(400, {{"error", "Invalid JSON"}});
        }
    });

    // 结构体绑定
    engine.Post("/api/products", [](gin::Context& ctx) {
        Product product;
        if (ctx.BindJSON(product)) {
            product.id = 1;
            ctx.JSON(201, {{"data", product}});
        }
    });

    // 带错误处理的 JSON
    engine.Post("/api/validate", [](gin::Context& ctx) {
        nlohmann::json body;
        if (!ctx.ShouldBindJSON(body)) {
            ctx.AbortWithStatusJSON(400, {{"error", "Invalid JSON format"}});
            return;
        }

        if (!body.contains("email") || !body["email"].is_string()) {
            ctx.AbortWithStatusJSON(400, {{"error", "Email is required"}});
            return;
        }

        ctx.JSON(200, {{"message", "Valid"}, {"email", body["email"]}});
    });

    // 数组响应
    engine.Get("/api/items", [](gin::Context& ctx) {
        nlohmann::json items = nlohmann::json::array({{{"id", 1}, {"name", "Item 1"}},
                                                      {{"id", 2}, {"name", "Item 2"}},
                                                      {{"id", 3}, {"name", "Item 3"}}});
        ctx.JSON(200, {{"items", items}, {"total", items.size()}});
    });

    // 带元数据的分页响应
    engine.Get("/api/list", [](gin::Context& ctx) {
        int page = std::stoi(ctx.DefaultQuery("page", "1"));
        int per_page = std::stoi(ctx.DefaultQuery("per_page", "10"));

        nlohmann::json items = nlohmann::json::array();
        for (int i = 0; i < per_page && i < 5; ++i) {
            items.push_back({{"id", (page - 1) * per_page + i + 1},
                             {"name", "Item " + std::to_string((page - 1) * per_page + i + 1)}});
        }

        ctx.JSON(200,
                 {{"data", items},
                  {"meta",
                   {{"page", page}, {"per_page", per_page}, {"total", 50}, {"total_pages", 5}}}});
    });

    std::cout << "JSON API example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Endpoints:" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/status" << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/status/pretty" << std::endl;
    std::cout
        << R"(  curl -X POST -H 'Content-Type: application/json' -d '{"key":"value"}' http://127.0.0.1:8080/api/echo)"
        << std::endl;
    std::cout
        << R"(  curl -X POST -H 'Content-Type: application/json' -d '{"name":"Widget","price":9.99,"tags":["sale"]}' http://127.0.0.1:8080/api/products)"
        << std::endl;
    std::cout << "  curl http://127.0.0.1:8080/api/items" << std::endl;
    std::cout << "  curl 'http://127.0.0.1:8080/api/list?page=2&per_page=3'" << std::endl;
    engine.Run(8080);

    return 0;
}
