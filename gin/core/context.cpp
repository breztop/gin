#include "core/context.hpp"

#include <fstream>
#include <functional>
#include <sstream>
#include <utility>

#include <nlohmann/json.hpp>

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

void Context::Header(const std::string& key, const std::string& value) {
    response.SetHeader(key, value);
}

void Context::SetCookie(const Cookie& cookie) { response.SetCookie(cookie); }

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
