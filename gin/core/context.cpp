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

std::string Context::DefaultQuery(const std::string& key, const std::string& default_value) const {
    auto value = request.GetQuery(key);
    return value.empty() ? default_value : value;
}

std::vector<std::string> Context::GetQueryArray(const std::string& key) const {
    std::vector<std::string> result;
    auto range = request.query_params.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second);
    }
    return result;
}

std::unordered_map<std::string, std::string> Context::GetQueryMap(const std::string& key) const {
    std::unordered_map<std::string, std::string> result;
    auto range = request.query_params.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        result[it->first] = it->second;
    }
    return result;
}

std::string Context::PostForm(const std::string& key) const { return request.GetPostForm(key); }

std::string Context::DefaultPostForm(const std::string& key,
                                     const std::string& default_value) const {
    auto value = request.GetPostForm(key);
    return value.empty() ? default_value : value;
}

std::vector<std::string> Context::GetPostFormArray(const std::string& key) const {
    std::vector<std::string> result;
    auto range = request.post_form.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second);
    }
    return result;
}

std::unordered_map<std::string, std::string> Context::GetPostFormMap(const std::string& key) const {
    std::unordered_map<std::string, std::string> result;
    auto range = request.post_form.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        result[it->first] = it->second;
    }
    return result;
}

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

std::any Context::MustGet(const std::string& key) {
    auto it = values_.find(key);
    if (it != values_.end()) {
        return it->second;
    }
    throw std::runtime_error("key not found: " + key);
}

bool Context::Delete(const std::string& key) { return values_.erase(key) > 0; }

std::string Context::GetString(const std::string& key) {
    auto val = Get(key);
    if (val.has_value()) {
        try {
            return std::any_cast<std::string>(val);
        } catch (...) {
            return "";
        }
    }
    return "";
}

int Context::GetInt(const std::string& key) {
    auto val = Get(key);
    if (val.has_value()) {
        try {
            return std::any_cast<int>(val);
        } catch (...) {
            try {
                return std::stoi(std::any_cast<std::string>(val));
            } catch (...) {
                return 0;
            }
        }
    }
    return 0;
}

double Context::GetFloat64(const std::string& key) {
    auto val = Get(key);
    if (val.has_value()) {
        try {
            return std::any_cast<double>(val);
        } catch (...) {
            try {
                return std::stod(std::any_cast<std::string>(val));
            } catch (...) {
                return 0.0;
            }
        }
    }
    return 0.0;
}

bool Context::GetBool(const std::string& key) {
    auto val = Get(key);
    if (val.has_value()) {
        try {
            return std::any_cast<bool>(val);
        } catch (...) {
            try {
                auto s = std::any_cast<std::string>(val);
                return s == "true" || s == "1";
            } catch (...) {
                return false;
            }
        }
    }
    return false;
}

void Context::Next() {
    if (handler_index_ < handlers_.size()) {
        handlers_[handler_index_++](*this);
    }
}

void Context::Abort() { aborted_ = true; }

void Context::AbortWithStatus(int code) {
    response.SetStatus(code);
    Abort();
}

void Context::AbortWithStatusJSON(int code, const nlohmann::json& obj) {
    JSON(code, obj);
    Abort();
}

bool Context::IsAborted() const { return aborted_; }

void Context::Status(int code) { response.SetStatus(code); }

void Context::FileAttachment(const std::string& filepath, const std::string& filename) {
    response.headers["Content-Disposition"] = "attachment; filename=\"" + filename + "\"";
    File(filepath);
}

void Context::FileFromFS(const std::string& filepath, const std::string& root) {
    std::string full_path = root + "/" + filepath;
    File(full_path);
}

std::vector<std::string> Context::HandlerNames() const {
    std::vector<std::string> names;
    for (const auto& handler : handlers_) {
        if (handler) {
            names.push_back("handler");
        }
    }
    return names;
}

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
