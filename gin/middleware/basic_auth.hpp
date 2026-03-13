#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {
namespace middleware {

inline std::string Base64Decode(const std::string& encoded) {
    const std::string chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string decoded;
    int val = 0, valb = -8;
    for (unsigned char c : encoded) {
        if (c == '=') break;
        auto idx = chars.find(c);
        if (idx == std::string::npos) continue;
        val = (val << 6) + idx;
        valb += 6;
        if (valb >= 0) {
            decoded += static_cast<char>((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    return decoded;
}

struct BasicAuthConfig {
    std::unordered_map<std::string, std::string> users;
    std::string realm = "Authorization Required";
};

inline Middleware BasicAuth(const BasicAuthConfig& config) {
    return [config](Context& ctx) {
        auto auth_header = ctx.request.GetHeader("Authorization");
        if (auth_header.empty()) {
            ctx.Header("WWW-Authenticate", "Basic realm=\"" + config.realm + "\"");
            ctx.AbortWithError(401, "Authorization required");
            return;
        }

        if (auth_header.substr(0, 6) != "Basic ") {
            ctx.AbortWithError(401, "Invalid authorization header");
            return;
        }

        std::string encoded = auth_header.substr(6);
        std::string decoded = Base64Decode(encoded);
        auto colon_pos = decoded.find(':');
        if (colon_pos == std::string::npos) {
            ctx.AbortWithError(401, "Invalid credentials format");
            return;
        }

        std::string username = decoded.substr(0, colon_pos);
        std::string password = decoded.substr(colon_pos + 1);

        auto it = config.users.find(username);
        if (it == config.users.end() || it->second != password) {
            ctx.AbortWithError(401, "Invalid credentials");
            return;
        }

        ctx.Set("username", username);
        ctx.Next();
    };
}

}  // namespace middleware
}  // namespace gin
