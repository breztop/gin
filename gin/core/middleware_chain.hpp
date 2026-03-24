#pragma once

#include <cstddef>
#include <vector>

#include "core/types.hpp"

namespace gin {


class Context;


class MiddlewareChain {
public:
    MiddlewareChain() = default;

    void Use(Middleware middleware);
    void SetHandlers(Handlers&& handlers);

    void Run(std::shared_ptr<Context> ctx);

private:
    std::vector<Middleware> middlewares_;
    size_t index_ = 0;
};

}  // namespace gin
