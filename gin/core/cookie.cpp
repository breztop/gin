#include "core/cookie.hpp"

namespace gin {

std::string Cookie::ToHeader() const {
    std::string result = name + "=" + value;
    
    if (!path.empty()) {
        result += "; Path=" + path;
    }
    if (!domain.empty()) {
        result += "; Domain=" + domain;
    }
    if (secure) {
        result += "; Secure";
    }
    if (http_only) {
        result += "; HttpOnly";
    }
    if (max_age > 0) {
        result += "; Max-Age=" + std::to_string(max_age);
    }
    if (!same_site.empty()) {
        result += "; SameSite=" + same_site;
    }
    
    return result;
}

}  // namespace gin
