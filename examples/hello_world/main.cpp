#include <iostream>
#include "core/app.hpp"

int main() {
    auto app = gin::App::Default();

    app.Get("/", [](gin::Context& ctx) {
        ctx.String(200, "Hello, World!");
    });

    std::cout << "Starting server on http://127.0.0.1:8080" << std::endl;
    app.Run(8080);

    return 0;
}
