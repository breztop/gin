#include "core/request.hpp"

#include <sstream>
#include <utility>

namespace gin {

std::string Request::GetHeader(const std::string& key) const {
    auto it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

std::string Request::GetParam(const std::string& key) const {
    auto it = params.find(key);
    if (it != params.end()) {
        return it->second;
    }
    return "";
}

std::string Request::GetQuery(const std::string& key) const {
    auto range = query_params.equal_range(key);
    if (range.first != range.second) {
        return range.first->second;
    }
    return "";
}

std::string Request::GetPostForm(const std::string& key) const {
    auto range = post_form.equal_range(key);
    if (range.first != range.second) {
        return range.first->second;
    }
    return "";
}

FormFile* Request::GetFile(const std::string& key) {
    for (auto& file : files) {
        if (file.filename == key || file.header_filename == key) {
            return &file;
        }
    }
    return nullptr;
}

const std::vector<FormFile>& Request::GetFiles() const { return files; }

bool Request::IsTLS() const { return protocol == "https" || protocol == "HTTPS"; }

void Request::ParseQueryParams() {
    if (query_string.empty()) {
        return;
    }

    std::istringstream ss(query_string);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        auto pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            query_params.emplace(key, value);
        } else {
            query_params.emplace(pair, "");
        }
    }
}

void Request::ParsePostForm() {
    if (body.empty()) {
        return;
    }

    std::istringstream ss(body);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        auto pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            post_form.emplace(key, value);
        } else {
            post_form.emplace(pair, "");
        }
    }
}

}  // namespace gin
