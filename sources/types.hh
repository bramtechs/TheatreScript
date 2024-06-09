#pragma once

#include <array>
#include <iostream>
#include <string>
#include <variant>

class Any : public std::variant<std::monostate, int, float, bool, std::string>
{
public:
    constexpr Any(const std::string_view& text) {
        if (text.empty()) {
            this->template emplace<0>();
        } else if (text.find_first_not_of("0123456789") == std::string_view::npos) {
            this->template emplace<1>(std::stoi(std::string(text)));
        } else if (text.find('.') != std::string_view::npos) {
            this->template emplace<2>(std::stof(std::string(text)));
        } else if (text == "true" || text == "false") {
            this->template emplace<3>(text == "true");
        } else {
            this->template emplace<4>(std::string(text));
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Any& any) {
        std::visit([&os](const auto& value) { os << to_string(value); }, any);
        return os;
    }

private:
    static std::string to_string(const std::string& s) {
        return s;
    }

    static std::string to_string(const std::monostate&) {
        return "(none)";
    }
    
    static std::string to_string(int value) {
        return std::to_string(value);
    }

    static std::string to_string(float value) {
        return std::to_string(value);
    }

    static std::string to_string(bool value) {
        return value ? "true" : "false";
    }
};
