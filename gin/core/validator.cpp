#include "core/validator.hpp"

#include <regex>

namespace gin {

bool Validator::Validate(const std::string& value, const std::string& tag) {
    if (tag.empty()) {
        return true;
    }

    if (tag == "required") {
        return ValidateRequired(value);
    }

    if (tag == "email") {
        return ValidateEmail(value);
    }

    size_t colon_pos = tag.find(':');
    if (colon_pos != std::string::npos) {
        std::string key = tag.substr(0, colon_pos);
        std::string param = tag.substr(colon_pos + 1);

        if (key == "min") {
            return ValidateMinLength(value, std::stoul(param));
        }
        if (key == "max") {
            return ValidateMaxLength(value, std::stoul(param));
        }
        if (key == "range") {
            size_t dash_pos = param.find('-');
            int min = std::stoi(param.substr(0, dash_pos));
            int max = std::stoi(param.substr(dash_pos + 1));
            return ValidateRange(value, min, max);
        }
        if (key == "regex") {
            return ValidatePattern(value, param);
        }
    }

    return true;
}

std::vector<ValidationError> Validator::ValidateStruct(void* obj, const std::string& json) {
    std::vector<ValidationError> errors;
    return errors;
}

bool Validator::BindJSON(const std::string& json, void* obj) {
    return false;
}

bool Validator::ValidateRequired(const std::string& value) {
    return !value.empty();
}

bool Validator::ValidateEmail(const std::string& value) {
    std::regex email_pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(value, email_pattern);
}

bool Validator::ValidateMinLength(const std::string& value, size_t min) {
    return value.length() >= min;
}

bool Validator::ValidateMaxLength(const std::string& value, size_t max) {
    return value.length() <= max;
}

bool Validator::ValidateRange(const std::string& value, int min, int max) {
    try {
        int num = std::stoi(value);
        return num >= min && num <= max;
    } catch (...) {
        return false;
    }
}

bool Validator::ValidatePattern(const std::string& value, const std::string& pattern) {
    try {
        std::regex re(pattern);
        return std::regex_match(value, re);
    } catch (...) {
        return false;
    }
}

}  // namespace gin
