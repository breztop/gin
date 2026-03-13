#include "core/form_file.hpp"

#include <fstream>

namespace gin {

std::string FormFile::GetFilename() const {
    return header_filename.empty() ? filename : header_filename;
}

std::string FormFile::GetContentType() const {
    return header_content_type;
}

const std::vector<uint8_t>& FormFile::GetContent() const {
    return content;
}

size_t FormFile::GetSize() const {
    return content.size();
}

bool FormFile::Save(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }
    out.write(reinterpret_cast<const char*>(content.data()), content.size());
    return out.good();
}

}  // namespace gin
