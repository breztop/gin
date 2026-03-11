#pragma once

#include <string>
#include <unordered_map>

namespace gin {

namespace Status {
    inline const std::unordered_map<int, std::string> Message = {
        {100, "Continue"},
        {101, "Switching Protocols"},
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {204, "No Content"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {304, "Not Modified"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {422, "Unprocessable Entity"},
        {429, "Too Many Requests"},
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
    };

    inline std::string MessageFor(int code) {
        auto it = Message.find(code);
        if (it != Message.end()) {
            return it->second;
        }
        return "Unknown";
    }
}

}
