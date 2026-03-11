#pragma once

#include <string>

namespace gin {

struct Cookie {
    std::string name;
    std::string value;
    std::string path = "/";
    std::string domain;
    bool secure = false;
    bool http_only = false;
    int max_age = -1;
    std::string same_site;

    std::string ToHeader() const;
};

}  // namespace gin
