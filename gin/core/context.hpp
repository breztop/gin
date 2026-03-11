#pragma once

#include <any>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "core/request.hpp"
#include "core/response.hpp"
#include "core/types.hpp"


namespace gin {


struct Cookie;
struct FormFile;


class Context {
public:
    Request request;
    Response response;

    explicit Context();

    std::string Param(const std::string& key) const;
    std::string Query(const std::string& key) const;
    std::string PostForm(const std::string& key) const;
    FormFile* GetFile(const std::string& key);
    const std::vector<FormFile>& FormFiles() const;

    void JSON(int status, const nlohmann::json& j);
    void String(int status, const std::string& str);
    void HTML(int status, const std::string& html);
    void File(const std::string& filepath);
    void Redirect(int status, const std::string& url);

    bool ShouldBindJSON(nlohmann::json& j);

    void Header(const std::string& key, const std::string& value);
    void SetCookie(const Cookie& cookie);

    void Set(const std::string& key, std::any value);
    std::any Get(const std::string& key);

    void Next();
    void Abort();
    bool IsAborted() const;

    void Error(int code, const std::string& message);
    void AbortWithError(int code, const std::string& message);

    void SetHandlers(Handlers&& handlers);
    void SetParams(std::unordered_map<std::string, std::string>&& params);

private:
    bool aborted_ = false;
    size_t handler_index_ = 0;
    Handlers handlers_;
    std::unordered_map<std::string, std::any> values_;
};

}  // namespace gin
