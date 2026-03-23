#pragma once

#include <random>
#include <sstream>
#include <string>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin::middleware {

inline std::string GenerateRequestID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << std::hex << dis(gen);
        if (i == 7 || i == 11 || i == 15 || i == 19) {
            ss << "-";
        }
    }
    return ss.str();
}

inline Middleware RequestID(const std::string& header = "X-Request-ID") {
    return [header](Context& ctx) {
        auto id = ctx.request.GetHeader(header);
        if (id.empty()) {
            id = GenerateRequestID();
        }
        ctx.Set("request_id", id);
        ctx.Header(header, id);
        ctx.Next();
    };
}

}  // namespace gin::middleware
