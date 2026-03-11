#pragma once

#include <string>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {

struct CORSOptions {
    std::string origin = "*";
    std::string methods = "GET, POST, PUT, DELETE, PATCH, OPTIONS";
    std::string headers = "Content-Type, Authorization";
    bool credentials = false;
    int max_age = 86400;
};

inline Middleware CORS(const CORSOptions& options = CORSOptions()) {
    return [options](Context& ctx) {
        ctx.Header("Access-Control-Allow-Origin", options.origin);
        ctx.Header("Access-Control-Allow-Methods", options.methods);
        ctx.Header("Access-Control-Allow-Headers", options.headers);
        ctx.Header("Access-Control-Max-Age", std::to_string(options.max_age));
        
        if (options.credentials) {
            ctx.Header("Access-Control-Allow-Credentials", "true");
        }
        
        if (ctx.request.method == "OPTIONS") {
            ctx.Abort();
            return;
        }
        
        ctx.Next();
    };
}

}  // namespace gin
