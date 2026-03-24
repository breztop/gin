#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace gin {

struct ValidationError {
    std::string field;
    std::string message;
};

class Validator {
public:
    static bool Validate(const std::string& value, const std::string& tag);

private:
    static bool ValidateRequired(const std::string& value);
    static bool ValidateEmail(const std::string& value);
    static bool ValidateMinLength(const std::string& value, size_t min);
    static bool ValidateMaxLength(const std::string& value, size_t max);
    static bool ValidateRange(const std::string& value, int min, int max);
    static bool ValidatePattern(const std::string& value, const std::string& pattern);
};

}  // namespace gin
