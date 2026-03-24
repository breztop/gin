// Form Binding Example
// 基于 gin-gonic/examples/form-binding 改编

#include <iostream>

#include "core/engine.hpp"

struct Booking {
    std::string name;
    std::string check_in;
    std::string check_out;
};

void from_json(const nlohmann::json& j, Booking& b) {
    if (j.contains("name")) j.at("name").get_to(b.name);
    if (j.contains("check_in")) j.at("check_in").get_to(b.check_in);
    if (j.contains("check_out")) j.at("check_out").get_to(b.check_out);
}

int main() {
    auto engine = gin::Engine::Default();

    // 显示表单页面
    engine->Get("/", [](gin::Context::Shared ctx) {
        std::string html = R"(
<!DOCTYPE html>
<html>
<head><title>Form Binding Example</title></head>
<body>
    <h1>Booking Form</h1>
    <form action="/book" method="post">
        <p>Name: <input type="text" name="name" required></p>
        <p>Check In: <input type="date" name="check_in" required></p>
        <p>Check Out: <input type="date" name="check_out"></p>
        <p><button type="submit">Book</button></p>
    </form>
</body>
</html>
)";
        ctx->HTML(200, html);
    });

    // 处理表单提交 - 使用 BindQuery
    engine->Get("/book", [](gin::Context::Shared ctx) {
        Booking booking;
        if (ctx->BindQuery(booking)) {
            ctx->JSON(200, {{"message", "Booking successful (GET)"},
                            {"name", booking.name},
                            {"check_in", booking.check_in}});
        }
    });

    // 处理表单提交 - 使用 BindJSON
    engine->Post("/book", [](gin::Context::Shared ctx) {
        // 尝试从 JSON 绑定
        nlohmann::json body;
        if (ctx->ContentType() == "application/json") {
            Booking booking;
            if (ctx->BindJSON(booking)) {
                ctx->JSON(200, {{"message", "Booking successful (JSON)"},
                                {"name", booking.name},
                                {"check_in", booking.check_in}});
            }
        } else {
            // 从表单绑定
            Booking booking;
            booking.name = ctx->PostForm("name");
            booking.check_in = ctx->PostForm("check_in");
            booking.check_out = ctx->PostForm("check_out");

            if (booking.name.empty() || booking.check_in.empty()) {
                ctx->JSON(400, {{"error", "Name and check_in are required"}});
                return;
            }

            ctx->JSON(200, {{"message", "Booking successful (Form)"},
                            {"name", booking.name},
                            {"check_in", booking.check_in},
                            {"check_out", booking.check_out}});
        }
    });

    std::cout << "Form binding example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Open http://127.0.0.1:8080 in your browser" << std::endl;
    std::cout << "Or try curl:" << std::endl;
    std::cout << "  curl -X POST -d 'name=John&check_in=2025-01-01' http://127.0.0.1:8080/book"
              << std::endl;
    std::cout
        << R"(  curl -H 'Content-Type: application/json' -d '{"name":"John","check_in":"2025-01-01"}' http://127.0.0.1:8080/book)"
        << std::endl;
    engine->Run(8080);

    return 0;
}
