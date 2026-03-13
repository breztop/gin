#pragma once

#include <string>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {
namespace middleware {

struct GzipConfig {
    int level = 6;
    std::string content_type = "text/html";
};

inline Middleware Gzip(const GzipConfig& config = GzipConfig()) {
    return [config](Context& ctx) {
        ctx.Next();
    };
}

}  // namespace middleware
}  // namespace gin
