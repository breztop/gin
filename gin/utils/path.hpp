#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace gin::utils {

inline std::string CleanPath(const std::string& path) {
    if (path.empty()) {
        return "/";
    }

    std::vector<std::string> parts;
    std::istringstream ss(path);
    std::string part;

    while (std::getline(ss, part, '/')) {
        if (part.empty() || part == ".") {
            continue;
        }
        if (part == "..") {
            if (!parts.empty() && parts.back() != "..") {
                parts.pop_back();
            }
        } else {
            parts.push_back(part);
        }
    }

    std::string result = "/";
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += "/";
        result += parts[i];
    }

    return result;
}

inline std::string JoinPaths(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;

    bool a_ends = a.back() == '/';
    bool b_starts = b.front() == '/';

    if (a_ends && b_starts) {
        return a + b.substr(1);
    } else if (!a_ends && !b_starts) {
        return a + "/" + b;
    }
    return a + b;
}

}  // namespace gin::utils
