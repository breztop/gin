#include "core/response.hpp"

#include <sstream>
#include <utility>

#include "core/status.hpp"

namespace gin {

void Response::Reset() {
    status_code = 200;
    body.clear();
    body_bytes.clear();
    headers.clear();
    cookies.clear();
}

void Response::SetStatus(int code) {
    status_code = code;
    SetDefaultHeaders();
}

void Response::SetHeader(const std::string& key, const std::string& value) { headers[key] = value; }

void Response::SetCookie(const Cookie& cookie) { cookies.push_back(cookie); }

void Response::SetDefaultHeaders() {
    if (headers.find("Content-Type") == headers.end()) {
        if (!body.empty()) {
            headers["Content-Type"] = "text/plain; charset=utf-8";
        }
    }
}

std::string Response::GetStatusLine() const {
    std::ostringstream ss;
    ss << "HTTP/1.1 " << status_code << " " << Status::MessageFor(status_code);
    return ss.str();
}

std::string Response::GetHeaders() const {
    std::ostringstream ss;
    for (const auto& [key, value] : headers) {
        ss << key << ": " << value << "\r\n";
    }
    for (const auto& cookie : cookies) {
        ss << "Set-Cookie: " << cookie.ToHeader() << "\r\n";
    }
    return ss.str();
}

}  // namespace gin
