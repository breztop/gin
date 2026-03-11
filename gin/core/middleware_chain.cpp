#include "core/middleware_chain.hpp"

#include <functional>
#include <utility>

#include "core/context.hpp"


namespace gin {

void MiddlewareChain::Use(Middleware middleware) { middlewares_.push_back(std::move(middleware)); }

void MiddlewareChain::SetHandlers(Handlers&& handlers) {
    for (auto& middleware : middlewares_) {
        handlers.insert(handlers.begin(), std::move(middleware));
    }
}

void MiddlewareChain::Run(Context& ctx) {
    index_ = 0;
    auto handlers = std::vector<Handler>{};

    for (auto& middleware : middlewares_) {
        handlers.push_back([&middleware](Context& c) {
            middleware(c);
        });
    }

    ctx.SetHandlers(std::move(handlers));

    if (!ctx.IsAborted()) {
        ctx.Next();
    }
}

}  // namespace gin
