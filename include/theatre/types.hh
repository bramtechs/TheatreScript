#pragma once

#include <array>
#include <iostream>
#include <string>
#include <variant>
#include <stdexcept>
#include <format>

namespace theatre {

using OperationError = std::runtime_error;
using NotImplementedError = std::exception;

enum class AnyType : int
{
    MONO,
    INT,
    FLOAT,
    BOOL,
    STRING
};

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
    
    bool IsMono() const {
        return std::holds_alternative<std::monostate>(*this);
    }

    template<typename T>
    bool IsType() const {
        return std::holds_alternative<T>(*this);
    }
    
    const char* GetTypeName() const {
        if (std::holds_alternative<int>(*this)) {
            return "int";
        }
        if (std::holds_alternative<float>(*this)) {
            return "float";
        }
        if (std::holds_alternative<std::string>(*this)) {
            return "string";
        }
        if (std::holds_alternative<std::monostate>(*this)) {
            return "mono";
        }
        throw NotImplementedError();
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

    inline void ThrowIfEitherIsString(const Any& any) const {
        // strings not allowed
        if (std::holds_alternative<std::string>(*this) || std::holds_alternative<std::string>(any)) {
            throw OperationError("Cannot subtract with strings");
        }
    }
        
#define OP_SHARED(O) \
    /* If either is float, type gets promoted */ \
    if (std::holds_alternative<float>(any) || std::holds_alternative<float>(*this)) { \
        return Any ( this->Value<float>() O any.Value<float>() ); \
    } else { \
        return Any ( this->Value<int>() O any.Value<int>() ); \
    } \
    
    Any operator +(const Any& any) const {
        // string concatenation
        if (std::holds_alternative<std::string>(*this) && std::holds_alternative<std::string>(any)) {
            return Any ( std::get<std::string>(*this) + std::get<std::string>(any) );
        }
    
        OP_SHARED(+)
    }
    
    Any operator -(const Any& any) const {
        ThrowIfEitherIsString(any);
        OP_SHARED(-)
    }
    
    Any operator *(const Any& any) const {
        ThrowIfEitherIsString(any);
        OP_SHARED(*)
    }
    
    Any operator /(const Any& any) const {
        ThrowIfEitherIsString(any);
        OP_SHARED(/)
    }

    // TODO: @cleanup inline
    template <typename T>
    T Value() const {
        if (std::holds_alternative<float>(*this)) {
            return (T)std::get<float>(*this);
        } else if (std::holds_alternative<int>(*this)) {
            return (T)std::get<int>(*this);
        } else {
            throw OperationError(std::format("Any {} does not contain valuable", ToString()));
        }
    }
    
private:
    static std::string ToString(const std::string& s) {
        return s;
    }

    static std::string ToString(const std::monostate&) {
        return "(mono)";
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

};