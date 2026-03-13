#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/form_file.hpp"

namespace gin {

class Request {
public:
    std::string method;
    std::string path;
    std::string query_string;
    std::string protocol;

    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> params;
    std::unordered_multimap<std::string, std::string> query_params;
    std::unordered_multimap<std::string, std::string> post_form;
    std::vector<FormFile> files;

    std::string body;
    std::vector<uint8_t> body_bytes;

    std::string GetHeader(const std::string& key) const;
    std::string GetParam(const std::string& key) const;
    std::string GetQuery(const std::string& key) const;
    std::string GetPostForm(const std::string& key) const;
    FormFile* GetFile(const std::string& key);
    const std::vector<FormFile>& GetFiles() const;
    bool IsTLS() const;

    void ParseQueryParams();
    void ParsePostForm();
    void ParseMultipartForm();
};

}  // namespace gin
