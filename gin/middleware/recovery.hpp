#pragma once

#include <exception>

#include "core/context.hpp"
#include "core/types.hpp"
#include "utils/logger.hpp"

namespace gin {

inline Middleware Recovery() {
    return [](std::shared_ptr<Context> ctx) {
        try {
            ctx->Next();
        } catch (const std::exception& e) {
            LOG_ERROR("Panic: {}", e.what());
            ctx->AbortWithError(500, "Internal Server Error");
        } catch (...) {
            LOG_ERROR("Unknown panic");
            ctx->AbortWithError(500, "Internal Server Error");
        }
    };
}

}  // namespace gin
