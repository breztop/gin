#pragma once

#include <any>
#include <cstddef>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/request.hpp"
#include "core/response.hpp"
#include "core/types.hpp"
#include "http/type.hpp"

namespace gin {


struct Cookie;
struct FormFile;


class Context : public std::enable_shared_from_this<Context> {
public:
    using Shared = std::shared_ptr<Context>;

    Request request;
    Response response;

    explicit Context();

    std::string Param(const std::string& key) const;
    std::string Query(const std::string& key) const;
    std::string DefaultQuery(const std::string& key, const std::string& default_value) const;
    std::vector<std::string> GetQueryArray(const std::string& key) const;
    std::unordered_map<std::string, std::string> GetQueryMap(const std::string& key) const;
    std::string PostForm(const std::string& key) const;
    std::string DefaultPostForm(const std::string& key, const std::string& default_value) const;
    std::vector<std::string> GetPostFormArray(const std::string& key) const;
    std::unordered_map<std::string, std::string> GetPostFormMap(const std::string& key) const;
    FormFile* GetFile(const std::string& key);
    const std::vector<FormFile>& FormFiles() const;

    void JSON(int status, const nlohmann::json& j);
    void JSON(HttpStatus status, const nlohmann::json& j);

    void IndentedJSON(int status, const nlohmann::json& j);
    void IndentedJSON(HttpStatus status, const nlohmann::json& j);

    void AsciiJSON(int status, const nlohmann::json& j);
    void AsciiJSON(HttpStatus status, const nlohmann::json& j);

    void PureJSON(int status, const nlohmann::json& j);
    void SecureJSON(int status, const nlohmann::json& j);
    void Negotiate(int status, const nlohmann::json& data);
    void Negotiate(int status, const std::string& format);
    void SaveUploadedFile(const std::string& key, const std::string& dest);
    void String(int status, const std::string& str);
    void String(HttpStatus status, const std::string& str);
    void HTML(int status, const std::string& html);
    void File(const std::string& filepath);
    void Data(int status, const std::string& content_type, const std::vector<uint8_t>& data);
    void Redirect(int status, const std::string& url);

    bool ShouldBindJSON(nlohmann::json& j);

    template <typename T>
    bool ShouldBind(T& obj);

    template <typename T>
    bool ShouldBindQuery(T& obj);

    template <typename T>
    bool ShouldBindHeader(T& obj);

    template <typename T>
    bool ShouldBindUri(T& obj);

    template <typename T>
    bool BindJSON(T& obj);

    template <typename T>
    bool BindQuery(T& obj);

    template <typename T>
    bool BindHeader(T& obj);

    template <typename T>
    bool BindUri(T& obj);

    std::string GetRawData() const;
    std::string ContentType() const;
    std::string Host() const;
    bool IsWebSocket() const;
    std::string ClientIP() const;
    std::string Url() const;
    std::string FullPath() const;
    std::string GetCookie(const std::string& name) const;

    void Header(const std::string& key, const std::string& value);
    void SetCookie(const Cookie& cookie);
    void DelCookie(const std::string& name);

    void Set(const std::string& key, std::any value);
    std::any Get(const std::string& key);
    std::any MustGet(const std::string& key);
    bool Delete(const std::string& key);

    std::string GetString(const std::string& key);
    int GetInt(const std::string& key);
    double GetFloat64(const std::string& key);
    bool GetBool(const std::string& key);

    void Next();
    void Abort();
    void AbortWithStatus(int code);
    void AbortWithStatusJSON(int code, const nlohmann::json& obj);
    bool IsAborted() const;

    void Error(int code, const std::string& message);
    void AbortWithError(int code, const std::string& message);

    void Status(int code);

    void FileAttachment(const std::string& filepath, const std::string& filename);
    void FileFromFS(const std::string& filepath, const std::string& root);

    std::vector<std::string> HandlerNames() const;

    void SetHandlers(Handlers&& handlers);
    void SetParams(std::unordered_map<std::string, std::string>&& params);

private:
    bool aborted_ = false;
    size_t handler_index_ = 0;
    Handlers handlers_;
    std::unordered_map<std::string, std::any> values_;
};

}  // namespace gin

namespace gin {

template <typename T>
bool Context::ShouldBind(T& obj) {
    nlohmann::json j;
    if (!ShouldBindJSON(j)) {
        return false;
    }
    try {
        nlohmann::from_json(j, obj);
        return true;
    } catch (...) {
        return false;
    }
}

template <typename T>
bool Context::ShouldBindQuery(T& obj) {
    try {
        nlohmann::json j;
        for (const auto& [key, value] : request.query_params) {
            j[key] = value;
        }
        nlohmann::from_json(j, obj);
        return true;
    } catch (...) {
        return false;
    }
}

template <typename T>
bool Context::ShouldBindHeader(T& obj) {
    try {
        nlohmann::json j;
        for (const auto& [key, value] : request.headers) {
            j[key] = value;
        }
        nlohmann::from_json(j, obj);
        return true;
    } catch (...) {
        return false;
    }
}

template <typename T>
bool Context::ShouldBindUri(T& obj) {
    try {
        nlohmann::json j;
        for (const auto& [key, value] : request.params) {
            j[key] = value;
        }
        nlohmann::from_json(j, obj);
        return true;
    } catch (...) {
        return false;
    }
}

template <typename T>
bool Context::BindJSON(T& obj) {
    if (ShouldBind(obj)) {
        return true;
    }
    AbortWithStatusJSON(400, {{"error", "invalid JSON"}});
    return false;
}

template <typename T>
bool Context::BindQuery(T& obj) {
    if (ShouldBindQuery(obj)) {
        return true;
    }
    AbortWithStatusJSON(400, {{"error", "invalid query parameters"}});
    return false;
}

template <typename T>
bool Context::BindHeader(T& obj) {
    if (ShouldBindHeader(obj)) {
        return true;
    }
    AbortWithStatusJSON(400, {{"error", "invalid headers"}});
    return false;
}

template <typename T>
bool Context::BindUri(T& obj) {
    if (ShouldBindUri(obj)) {
        return true;
    }
    AbortWithStatusJSON(400, {{"error", "invalid uri parameters"}});
    return false;
}

}  // namespace gin
