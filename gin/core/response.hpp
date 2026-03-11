#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/cookie.hpp"

namespace gin {

class Response {
public:
    int status_code = 200;
    std::string body;
    std::vector<uint8_t> body_bytes;

    std::unordered_map<std::string, std::string> headers;
    std::vector<Cookie> cookies;

    void SetStatus(int code);
    void SetHeader(const std::string& key, const std::string& value);
    void SetCookie(const Cookie& cookie);
    void Reset();

    std::string GetStatusLine() const;
    std::string GetHeaders() const;

private:
    void SetDefaultHeaders();
};

}  // namespace gin
