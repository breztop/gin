#pragma once

#include "core/context.hpp"
#include "core/types.hpp"
#include "utils/logger.hpp"

namespace gin {

inline Middleware Logger() {
    return [](gin::Context::Shared ctx) {
        LOG_INFO("{} {} {}", ctx->request.method, ctx->request.path, ctx->response.status_code);
        ctx->Next();
    };
}

}  // namespace gin
