#include <iostream>

#include "core/engine.hpp"

int main() {
    auto engine = gin::Engine::Default();

    engine->Get("/", [](gin::Context::Shared ctx) {
        ctx->String(200, "Hello, World!");
    });

    std::cout << "Starting server on http://127.0.0.1:8080" << std::endl;
    engine->Run(8080);

    return 0;
}
