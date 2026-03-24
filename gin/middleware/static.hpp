#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {

namespace {

std::string GetMimeType(const std::string& ext) {
    static const std::unordered_map<std::string, std::string> mime_types = {
        {".html", "text/html; charset=utf-8"},
        {".htm", "text/html; charset=utf-8"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".txt", "text/plain"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".svg", "image/svg+xml"},
        {".ico", "image/x-icon"},
        {".webp", "image/webp"},
        {".mp4", "video/mp4"},
        {".webm", "video/webm"},
        {".mp3", "audio/mpeg"},
        {".wav", "audio/wav"},
        {".woff", "font/woff"},
        {".woff2", "font/woff2"},
        {".ttf", "font/ttf"},
        {".pdf", "application/pdf"},
        {".zip", "application/zip"},
    };
    auto it = mime_types.find(ext);
    return it != mime_types.end() ? it->second : "application/octet-stream";
}

}  // namespace

inline Middleware Static(const std::string& prefix, const std::string& root) {
    return [prefix, root](gin::Context::Shared ctx) {
        std::string path = ctx->request.path;

        if (path.find(prefix) != 0) {
            ctx->Next();
            return;
        }

        std::string file_path = path.substr(prefix.size());
        if (file_path.empty() || file_path[0] != '/') {
            file_path = "/" + file_path;
        }

        file_path = root + file_path;

        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            ctx->AbortWithError(404, "File not found");
            return;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        ctx->response.body = ss.str();

        size_t pos = file_path.find_last_of('.');
        if (pos != std::string::npos) {
            std::string ext = file_path.substr(pos);
            ctx->response.headers["Content-Type"] = GetMimeType(ext);
        }

        ctx->response.status_code = 200;
    };
}

}  // namespace gin
