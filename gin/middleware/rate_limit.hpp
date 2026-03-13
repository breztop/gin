#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {
namespace middleware {

struct RateLimitConfig {
    int requests_per_second = 10;
    int burst = 20;
    std::string key_func = "ip";
};

struct TokenBucket {
    int tokens;
    int max_tokens;
    std::chrono::steady_clock::time_point last_update;

    TokenBucket(int capacity) : tokens(capacity), max_tokens(capacity) {
        last_update = std::chrono::steady_clock::now();
    }

    bool Consume() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
        
        tokens = std::min(max_tokens, tokens + static_cast<int>(elapsed));
        last_update = now;

        if (tokens > 0) {
            tokens--;
            return true;
        }
        return false;
    }
};

class RateLimiter {
public:
    static RateLimiter& Instance() {
        static RateLimiter instance;
        return instance;
    }

    bool Allow(const std::string& key, int capacity) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        
        auto it = buckets_.find(key);
        if (it == buckets_.end()) {
            buckets_[key] = TokenBucket(capacity);
            it = buckets_.find(key);
        }
        
        return it->second.Consume();
    }

    void Cleanup() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        
        for (auto it = buckets_.begin(); it != buckets_.end();) {
            auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - it->second.last_update).count();
            if (elapsed > 5) {
                it = buckets_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::unordered_map<std::string, TokenBucket> buckets_;
    std::shared_mutex mutex_;
};

inline Middleware RateLimit(const RateLimitConfig& config) {
    return [config](Context& ctx) {
        std::string key;

        if (config.key_func == "ip") {
            key = ctx.request.GetHeader("X-Forwarded-For");
            if (key.empty()) {
                key = ctx.request.GetHeader("X-Real-IP");
            }
            if (key.empty()) {
                key = "default";
            }
        } else if (config.key_func == "api_key") {
            auto api_key = ctx.request.GetHeader("X-API-Key");
            if (!api_key.empty()) {
                key = "api:" + api_key;
            } else {
                key = "default";
            }
        } else {
            key = "default";
        }

        if (!RateLimiter::Instance().Allow(key, config.burst)) {
            ctx.AbortWithError(429, "Too Many Requests");
            ctx.Header("Retry-After", "1");
            return;
        }

        ctx.Next();
    };
}

}  // namespace middleware
}  // namespace gin
