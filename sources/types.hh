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
    
    template<typename T>
    T Extract() const {
        if (!std::holds_alternative<T>(*this)) {
            throw OperationError(std::format("Expected type {}, but got {}", typeid(T).name(), ToString()));
        }
        return std::get<T>(*this);
    }
    
    bool IsUndefined() const {
        return std::holds_alternative<std::monostate>(*this);
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
        // string concatenation
        if (std::holds_alternative<std::string>(*this) && std::holds_alternative<std::string>(any)) {
            return Any ( std::get<std::string>(*this) + std::get<std::string>(any) );
        } else if (std::holds_alternative<std::monostate>(*this) && std::holds_alternative<std::string>(any)) {
            return Any ( std::get<std::string>(any) );
        } else {
            // If either is float, type gets promoted
            if (std::holds_alternative<float>(any) || std::holds_alternative<float>(*this)) {
                return Any ( this->Value<float>() + any.Value<float>() );
            } else {
                return Any ( this->Value<int>() + any.Value<int>() );
            }
        }
    }

    // TODO: @cleanup inline
    template <typename T>
    T Value() const {
        if (std::holds_alternative<float>(*this)) {
            return (T)std::get<float>(*this);
        } else if (std::holds_alternative<int>(*this)) {
            return (T)std::get<int>(*this);
        } else if (std::holds_alternative<std::monostate>(*this)) {
            return (T)0.f;
        } else {
            throw OperationError(std::format("Any {} does not contain valuable", ToString()));
        }
    }
    
private:
    static std::string ToString(const std::string& s) {
        return std::format("'{}'", s);
    }

    static std::string ToString(const std::monostate&) {
        return "(undefined)";
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
