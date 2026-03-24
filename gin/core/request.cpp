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

    auto content_type = GetHeader("Content-Type");
    if (content_type.find("multipart/form-data") != std::string::npos) {
        ParseMultipartForm();
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

void Request::ParseMultipartForm() {
    auto content_type = GetHeader("Content-Type");
    auto boundary_pos = content_type.find("boundary=");
    if (boundary_pos == std::string::npos) {
        return;
    }
    
    std::string boundary = content_type.substr(boundary_pos + 9);
    while (!boundary.empty() && (boundary[0] == ' ' || boundary[0] == '"' || boundary[0] == '\r' || boundary[0] == '\n')) {
        boundary = boundary.substr(1);
    }
    while (!boundary.empty() && (boundary.back() == ' ' || boundary.back() == '"' || boundary.back() == '\r' || boundary.back() == '\n')) {
        boundary.pop_back();
    }
    
    std::string delimiter = "--" + boundary;
    
    size_t pos = 0;
    while (pos < body.size()) {
        auto delim_pos = body.find(delimiter, pos);
        if (delim_pos == std::string::npos) break;
        
        auto section_start = delim_pos + delimiter.size();
        if (section_start >= body.size()) break;
        
        if (body.substr(section_start, 2) == "--") {
            break;
        }
        
        if (body.substr(section_start, 2) == "\r\n") {
            section_start += 2;
        }
        
        auto next_delim = body.find(delimiter, section_start);
        if (next_delim == std::string::npos) break;
        
        auto content_start = body.find("\r\n\r\n", section_start);
        if (content_start == std::string::npos || content_start >= next_delim) {
            pos = next_delim + delimiter.size();
            continue;
        }
        
        std::string header_section = body.substr(section_start, content_start - section_start);
        std::string content_section = body.substr(content_start + 4, next_delim - content_start - 4);
        
        auto name_pos = header_section.find("name=\"");
        auto filename_pos = header_section.find("filename=\"");
        
        FormFile file;
        
        if (filename_pos != std::string::npos) {
            auto filename_start = filename_pos + 10;
            auto filename_end = header_section.find("\"", filename_start);
            if (filename_end != std::string::npos) {
                file.header_filename = header_section.substr(filename_start, filename_end - filename_start);
            }
            
            auto name_start = name_pos + 6;
            auto name_end = header_section.find("\"", name_start);
            if (name_end != std::string::npos) {
                file.filename = header_section.substr(name_start, name_end - name_start);
            }
            
            auto content_type_pos = header_section.find("Content-Type:");
            if (content_type_pos != std::string::npos) {
                auto ct_start = header_section.find_first_not_of(" \t", content_type_pos + 13);
                auto ct_end = header_section.find("\r\n", ct_start);
                if (ct_end != std::string::npos) {
                    file.header_content_type = header_section.substr(ct_start, ct_end - ct_start);
                }
            }
            
            file.content.assign(content_section.begin(), content_section.end());
            
            if (!file.header_filename.empty()) {
                files.push_back(file);
            }
        } else if (name_pos != std::string::npos) {
            auto name_start = name_pos + 6;
            auto name_end = header_section.find("\"", name_start);
            if (name_end != std::string::npos) {
                std::string name = header_section.substr(name_start, name_end - name_start);
                post_form.emplace(name, content_section);
            }
        }
        
        pos = next_delim + delimiter.size();
    }
}

}  // namespace gin
