#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace gin {

struct FormFile {
    std::string filename;
    std::string header_filename;
    std::string header_content_type;
    std::vector<uint8_t> content;

    std::string GetFilename() const;
    std::string GetContentType() const;
    const std::vector<uint8_t>& GetContent() const;
    size_t GetSize() const;
    bool Save(const std::string& path) const;
};

}  // namespace gin
