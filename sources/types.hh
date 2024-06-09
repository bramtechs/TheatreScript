#pragma once

#include <array>
#include <iostream>
#include <string>
#include <variant>
#include <stdexcept>

using OperationError = std::runtime_error;

using AnyVariant = std::variant<std::monostate, int, float, bool, std::string>;
class Any : public AnyVariant
{
public:
    constexpr Any() = default;
    constexpr Any(std::monostate) : AnyVariant(std::monostate{}) {}
    constexpr Any(int value) : AnyVariant(value) {}
    constexpr Any(float value) : AnyVariant(value) {}
    constexpr Any(bool value) : AnyVariant(value) {}
    constexpr Any(const std::string& value) : AnyVariant(value) {}
    constexpr Any(const std::string_view& value) : Any(std::string(value)) {}

    template<std::size_t N>
    constexpr Any(const char (&value)[N]) : Any(std::string(value)) {}
    
    // Additional constructors for convenience
    Any(const char* value) : Any(std::string(value)) {}

    static Any Parse(const std::string& text) {
        if (text.empty()) {
            return Any();
        } else if (text.find_first_not_of("0123456789") == std::string_view::npos) {
            return Any(std::stoi(text));
        } else if (text.find('.') != std::string_view::npos) {
            return Any(std::stof(text));
        } else if (text == "true" || text == "false") {
            return Any(text == "true");
        } else {
            return Any(text);
        }
    }
    
    std::string ToString() const {
        std::string result;
        std::visit([&](const auto& value) { result = ToString(value); }, *this);
        return result;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Any& any) {
        std::visit([&os](const auto& value) { os << ToString(value); }, any);
        return os;
    }

    Any operator +(const Any& any) const {
        throw OperationError("Not implemented");
    }

    
private:
    static std::string ToString(const std::string& s) {
        return s;
    }

    static std::string ToString(const std::monostate&) {
        return "(none)";
    }
    
    static std::string ToString(int value) {
        return std::to_string(value);
    }

    static std::string ToString(float value) {
        return std::to_string(value);
    }

    static std::string ToString(bool value) {
        return value ? "true" : "false";
    }
};
