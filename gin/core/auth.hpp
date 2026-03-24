#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {

inline const std::string AuthUserKey = "user";

inline const std::string AuthProxyUserKey = "proxy_user";

using Accounts = std::unordered_map<std::string, std::string>;

struct AuthPair {
    std::string value;
    std::string user;
};

using AuthPairs = std::vector<AuthPair>;

namespace detail {

inline std::string Base64Encode(const std::string& input) {
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    result.reserve(((input.size() + 2) / 3) * 4);
    for (size_t i = 0; i < input.size(); i += 3) {
        uint32_t n = static_cast<uint32_t>(static_cast<unsigned char>(input[i])) << 16;
        if (i + 1 < input.size()) {
            n |= static_cast<uint32_t>(static_cast<unsigned char>(input[i + 1])) << 8;
        }
        if (i + 2 < input.size()) {
            n |= static_cast<uint32_t>(static_cast<unsigned char>(input[i + 2]));
        }
        result += table[(n >> 18) & 0x3F];
        result += table[(n >> 12) & 0x3F];
        result += (i + 1 < input.size()) ? table[(n >> 6) & 0x3F] : '=';
        result += (i + 2 < input.size()) ? table[n & 0x3F] : '=';
    }
    return result;
}

inline std::string Base64Decode(const std::string& encoded) {
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string decoded;
    int val = 0, valb = -8;
    for (unsigned char c : encoded) {
        if (c == '=') break;
        auto idx = chars.find(c);
        if (idx == std::string::npos) continue;
        val = (val << 6) + static_cast<int>(idx);
        valb += 6;
        if (valb >= 0) {
            decoded += static_cast<char>((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    return decoded;
}

inline int ConstantTimeCompare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return 0;
    }
    volatile unsigned char result = 0;
    for (size_t i = 0; i < a.size(); i++) {
        result |= static_cast<unsigned char>(a[i]) ^ static_cast<unsigned char>(b[i]);
    }
    return (1 & ((result - 1) >> 8));
}

inline std::string SearchCredential(const AuthPairs& pairs, const std::string& authValue,
                                    std::string& user) {
    if (authValue.empty()) {
        return "";
    }
    for (const auto& pair : pairs) {
        if (ConstantTimeCompare(pair.value, authValue) == 1) {
            user = pair.user;
            return user;
        }
    }
    return "";
}

inline AuthPairs ProcessAccounts(const Accounts& accounts) {
    if (accounts.empty()) {
        throw std::runtime_error("Empty list of authorized credentials");
    }
    AuthPairs pairs;
    pairs.reserve(accounts.size());
    for (const auto& [user, password] : accounts) {
        if (user.empty()) {
            throw std::runtime_error("User can not be empty");
        }
        std::string base = user + ":" + password;
        pairs.push_back({"Basic " + Base64Encode(base), user});
    }
    return pairs;
}

}  // namespace detail

inline Middleware BasicAuthForRealm(const Accounts& accounts, const std::string& realm) {
    std::string effective_realm = realm;
    if (effective_realm.empty()) {
        effective_realm = "Authorization Required";
    }
    effective_realm = "Basic realm=\"" + effective_realm + "\"";
    auto pairs = detail::ProcessAccounts(accounts);
    return [pairs = std::move(pairs), realm = std::move(effective_realm)](Context::Shared ctx) {
        std::string user;
        detail::SearchCredential(pairs, ctx->request.GetHeader("Authorization"), user);
        if (user.empty()) {
            ctx->Header("WWW-Authenticate", realm);
            ctx->AbortWithStatus(401);
            return;
        }
        ctx->Set(AuthUserKey, user);
        ctx->Next();
    };
}

inline Middleware BasicAuth(const Accounts& accounts) { return BasicAuthForRealm(accounts, ""); }

inline Middleware BasicAuthForProxy(const Accounts& accounts, const std::string& realm) {
    std::string effective_realm = realm;
    if (effective_realm.empty()) {
        effective_realm = "Proxy Authorization Required";
    }
    effective_realm = "Basic realm=\"" + effective_realm + "\"";
    auto pairs = detail::ProcessAccounts(accounts);
    return [pairs = std::move(pairs), realm = std::move(effective_realm)](Context::Shared ctx) {
        std::string proxy_user;
        detail::SearchCredential(pairs, ctx->request.GetHeader("Proxy-Authorization"), proxy_user);
        if (proxy_user.empty()) {
            ctx->Header("Proxy-Authenticate", realm);
            ctx->AbortWithStatus(407);
            return;
        }
        ctx->Set(AuthProxyUserKey, proxy_user);
        ctx->Next();
    };
}

namespace middleware {

struct BasicAuthConfig {
    std::unordered_map<std::string, std::string> users;
    std::string realm = "Authorization Required";
};

inline Middleware BasicAuth(const BasicAuthConfig& config) {
    return BasicAuthForRealm(config.users, config.realm);
}

}  // namespace middleware

}  // namespace gin
