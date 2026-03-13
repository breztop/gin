#pragma once

#include <functional>
#include <string>
#include <vector>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {
namespace middleware {

struct KeyAuthConfig {
    std::string key_name = "key";
    std::vector<std::string> keys;
    std::string header_name = "X-API-Key";
    std::string query_name = "api_key";
};

inline Middleware KeyAuth(const KeyAuthConfig& config) {
    return [config](Context& ctx) {
        std::string api_key;

        auto header_key = ctx.request.GetHeader(config.header_name);
        if (!header_key.empty()) {
            api_key = header_key;
        } else {
            api_key = ctx.Query(config.query_name);
        }

        if (api_key.empty()) {
            ctx.AbortWithError(401, "API key required");
            return;
        }

        bool valid = false;
        for (const auto& key : config.keys) {
            if (key == api_key) {
                valid = true;
                break;
            }
        }

        if (!valid) {
            ctx.AbortWithError(401, "Invalid API key");
            return;
        }

        ctx.Set("api_key", api_key);
        ctx.Next();
    };
}

}  // namespace middleware
}  // namespace gin
