#include "core/context.hpp"

#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <sstream>
#include <utility>

namespace gin {
struct FormFile;
struct Cookie;

}  // namespace gin

namespace gin {

Context::Context() = default;

std::string Context::Param(const std::string& key) const { return request.GetParam(key); }

std::string Context::Query(const std::string& key) const { return request.GetQuery(key); }

std::string Context::PostForm(const std::string& key) const { return request.GetPostForm(key); }

FormFile* Context::GetFile(const std::string& key) { return request.GetFile(key); }

const std::vector<FormFile>& Context::FormFiles() const { return request.GetFiles(); }

void Context::JSON(int status, const nlohmann::json& j) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "application/json";
    response.body = j.dump();
}

void Context::IndentedJSON(int status, const nlohmann::json& j) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "application/json";
    response.body = j.dump(4);
}

void Context::AsciiJSON(int status, const nlohmann::json& j) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "application/json";
    std::function<std::string(const nlohmann::json&)> escape_ascii =
        [&escape_ascii](const nlohmann::json& j) -> std::string {
        if (j.is_string()) {
            std::string s = j.get<std::string>();
            std::string result;
            for (char32_t c : s) {
                if (c < 128) {
                    result += c;
                } else {
                    char buf[7] = "\\u00  ";
                    snprintf(buf + 4, 3, "%02x", static_cast<unsigned char>(c));
                    result += buf;
                }
            }
            return result;
        } else if (j.is_array()) {
            std::string result = "[";
            for (size_t i = 0; i < j.size(); ++i) {
                result += escape_ascii(j[i]);
                if (i < j.size() - 1) result += ",";
            }
            result += "]";
            return result;
        } else if (j.is_object()) {
            std::string result = "{";
            bool first = true;
            for (auto& [key, value] : j.items()) {
                if (!first) result += ",";
                result += "\"" + key + "\":" + escape_ascii(value);
                first = false;
            }
            result += "}";
            return result;
        }
        return j.dump();
    };
    response.body = escape_ascii(j);
}

void Context::PureJSON(int status, const nlohmann::json& j) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "application/json";
    response.body = j.dump();
}

void Context::String(int status, const std::string& str) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "text/plain; charset=utf-8";
    response.body = str;
}

void Context::HTML(int status, const std::string& html) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "text/html; charset=utf-8";
    response.body = html;
}

void Context::File(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        AbortWithError(404, "File not found");
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    response.body = ss.str();

    size_t pos = filepath.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = filepath.substr(pos);
        if (ext == ".html" || ext == ".htm") {
            response.headers["Content-Type"] = "text/html; charset=utf-8";
        } else if (ext == ".css") {
            response.headers["Content-Type"] = "text/css";
        } else if (ext == ".js") {
            response.headers["Content-Type"] = "application/javascript";
        } else if (ext == ".json") {
            response.headers["Content-Type"] = "application/json";
        } else if (ext == ".png") {
            response.headers["Content-Type"] = "image/png";
        } else if (ext == ".jpg" || ext == ".jpeg") {
            response.headers["Content-Type"] = "image/jpeg";
        } else if (ext == ".svg") {
            response.headers["Content-Type"] = "image/svg+xml";
        }
    }
}

void Context::Data(int status, const std::string& content_type, const std::vector<uint8_t>& data) {
    response.SetStatus(status);
    response.headers["Content-Type"] = content_type;
    response.body_bytes = data;
}

void Context::Redirect(int status, const std::string& url) {
    response.SetStatus(status);
    response.headers["Location"] = url;
}

bool Context::ShouldBindJSON(nlohmann::json& j) {
    try {
        j = nlohmann::json::parse(request.body);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Context::GetRawData() const { return request.body; }

std::string Context::ContentType() const { return request.GetHeader("Content-Type"); }

std::string Context::Host() const { return request.GetHeader("Host"); }

bool Context::IsWebSocket() const {
    auto connection = request.GetHeader("Connection");
    auto upgrade = request.GetHeader("Upgrade");
    return connection == "Upgrade" && upgrade == "websocket";
}

std::string Context::ClientIP() const {
    auto ip = request.GetHeader("X-Forwarded-For");
    if (!ip.empty()) {
        auto comma_pos = ip.find(',');
        if (comma_pos != std::string::npos) {
            return ip.substr(0, comma_pos);
        }
        return ip;
    }
    ip = request.GetHeader("X-Real-IP");
    if (!ip.empty()) {
        return ip;
    }
    return "";
}

std::string Context::Url() const {
    return request.path + (request.query_string.empty() ? "" : "?" + request.query_string);
}

std::string Context::FullPath() const { return Url(); }

std::string Context::GetCookie(const std::string& name) const {
    for (const auto& cookie : response.cookies) {
        if (cookie.name == name) {
            return cookie.value;
        }
    }
    return "";
}

void Context::SecureJSON(int status, const nlohmann::json& j) {
    response.SetStatus(status);
    response.headers["Content-Type"] = "application/json";
    response.headers["X-Content-Type-Options"] = "nosniff";
    if (j.is_array()) {
        response.body = "while(1);" + j.dump();
    } else {
        response.body = j.dump();
    }
}

void Context::Negotiate(int status, const nlohmann::json& data) {
    auto accept = request.GetHeader("Accept");
    if (accept.find("application/json") != std::string::npos) {
        JSON(status, data);
    } else {
        String(status, data.dump());
    }
}

void Context::Negotiate(int status, const std::string& format) {
    auto accept = request.GetHeader("Accept");
    if (accept.find("application/json") != std::string::npos || format == "json") {
        auto j = nlohmann::json::parse(request.body);
        JSON(status, j);
    } else {
        String(status, request.body);
    }
}

void Context::SaveUploadedFile(const std::string& key, const std::string& dest) {
    auto file = GetFile(key);
    if (!file) {
        return;
    }
    std::ofstream out(dest, std::ios::binary);
    if (out) {
        out.write(reinterpret_cast<const char*>(file->content.data()), file->content.size());
    }
}

void Context::Header(const std::string& key, const std::string& value) {
    response.SetHeader(key, value);
}

void Context::SetCookie(const Cookie& cookie) { response.SetCookie(cookie); }

void Context::DelCookie(const std::string& name) {
    Cookie cookie;
    cookie.name = name;
    cookie.value = "";
    cookie.max_age = 0;
    response.SetCookie(cookie);
}

void Context::Set(const std::string& key, std::any value) { values_[key] = std::move(value); }

std::any Context::Get(const std::string& key) {
    auto it = values_.find(key);
    if (it != values_.end()) {
        return it->second;
    }
    return std::any();
}

void Context::Next() {
    if (handler_index_ < handlers_.size()) {
        handlers_[handler_index_++](*this);
    }
}

void Context::Abort() { aborted_ = true; }

bool Context::IsAborted() const { return aborted_; }

void Context::Error(int code, const std::string& message) {
    response.SetStatus(code);
    response.body = message;
}

void Context::AbortWithError(int code, const std::string& message) {
    Abort();
    Error(code, message);
}

void Context::SetHandlers(Handlers&& handlers) { handlers_ = std::move(handlers); }

void Context::SetParams(std::unordered_map<std::string, std::string>&& params) {
    request.params = std::move(params);
}

}  // namespace gin
