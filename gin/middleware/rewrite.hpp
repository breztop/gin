#pragma once

#include <functional>
#include <regex>
#include <string>
#include <vector>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {
namespace middleware {

struct RewriteRule {
    std::regex pattern;
    std::string replacement;
};

struct RewriteConfig {
    std::vector<RewriteRule> rules;

    RewriteConfig& AddRule(const std::string& pattern, const std::string& replacement) {
        RewriteRule rule;
        rule.pattern = std::regex(pattern);
        rule.replacement = replacement;
        rules.push_back(rule);
        return *this;
    }
};

inline Middleware Rewrite(const RewriteConfig& config) {
    return [config](Context& ctx) {
        std::string original_path = ctx.request.path;
        
        for (const auto& rule : config.rules) {
            std::string new_path = std::regex_replace(original_path, rule.pattern, rule.replacement);
            if (new_path != original_path) {
                ctx.request.path = new_path;
                break;
            }
        }

        ctx.Next();
    };
}

}  // namespace middleware
}  // namespace gin
