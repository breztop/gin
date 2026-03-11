#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "core/context.hpp"
#include "core/types.hpp"

namespace gin {

inline Middleware Static(const std::string& prefix, const std::string& root) {
    return [prefix, root](Context& ctx) {
        std::string path = ctx.request.path;

        if (path.find(prefix) != 0) {
            ctx.Next();
            return;
        }

        std::string file_path = path.substr(prefix.size());
        if (file_path.empty() || file_path[0] != '/') {
            file_path = "/" + file_path;
        }

        file_path = root + file_path;

        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            ctx.AbortWithError(404, "File not found");
            return;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        ctx.response.body = ss.str();

        size_t pos = file_path.find_last_of('.');
        if (pos != std::string::npos) {
            std::string ext = file_path.substr(pos);
            if (ext == ".html" || ext == ".htm") {
                ctx.response.headers["Content-Type"] = "text/html; charset=utf-8";
            } else if (ext == ".css") {
                ctx.response.headers["Content-Type"] = "text/css";
            } else if (ext == ".js") {
                ctx.response.headers["Content-Type"] = "application/javascript";
            } else if (ext == ".json") {
                ctx.response.headers["Content-Type"] = "application/json";
            } else if (ext == ".png") {
                ctx.response.headers["Content-Type"] = "image/png";
            } else if (ext == ".jpg" || ext == ".jpeg") {
                ctx.response.headers["Content-Type"] = "image/jpeg";
            } else if (ext == ".svg") {
                ctx.response.headers["Content-Type"] = "image/svg+xml";
            } else if (ext == ".ico") {
                ctx.response.headers["Content-Type"] = "image/x-icon";
            } else if (ext == ".txt") {
                ctx.response.headers["Content-Type"] = "text/plain";
            }
        }

        ctx.response.status_code = 200;
    };
}

}  // namespace gin
