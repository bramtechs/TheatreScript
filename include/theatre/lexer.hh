#pragma once
#include <string_view>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <stdexcept>
#include <magic_enum.hpp>

namespace theatre
{
    using LexerError = std::runtime_error;

    enum class TokenType
    {
        FN,
        IDENT,
        LITERAL,
        PAREN_OPEN,
        PAREN_CLOSE,
        COMMA,
        TYPE,
        BRACE_OPEN,
        BRACE_CLOSE,
        FOR,
        EQUALS,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        SEMICOLON,
        LESS_THAN,
        GREATER_THAN,
        RETURN,
        MUT,
    };

    constexpr size_t MAX_TOKEN_LEN = 64;
    struct StackString : public std::array<char, MAX_TOKEN_LEN>
    {
        constexpr StackString() {
            (*this)[0] = '\0';
        }

        constexpr StackString(const std::string_view &text)
        {
            for (size_t i = 0; i < MAX_TOKEN_LEN; ++i)
            {
                (*this)[i] = '\0';
            }

            size_t i = 0;
            while (i < text.size() && i < MAX_TOKEN_LEN - 1)
            {
                (*this)[i] = text[i];
                ++i;
            }

            (*this)[i] = '\0';
        }

        constexpr size_t Length() const {
            return std::string_view(data()).size();
        }

        constexpr operator std::string_view() const
        {
            return std::string_view(data());
        }

        friend std::ostream& operator<<(std::ostream& os, const StackString& str) {
            os << std::string_view(str);
            return os;
        }
    };

    template <TokenType type>
    constexpr bool HasDefaultTokenValue();

    template <TokenType type>
    constexpr StackString GetDefaultTokenValue();

    struct Token
    {
        const TokenType type;
        const StackString value;
        int col{-1};
        int row{-1};

        constexpr Token(TokenType type, const std::string_view &value)
            : type(type), value(value)
        {
        }

        constexpr Token(const Token& o)
            : type(o.type), value(o.value), col(o.col), row(o.row)
        {
        }

        constexpr Token &At(int col, int row)
        {
            this->col = col;
            this->row = row;
            return *this;
        }

        inline constexpr bool IsType(TokenType type) const
        {
            return this->type == type;
        }

        std::string ToString() const {
            std::stringstream s;
            s << *this;
            return s.str();
        }

        // NOTE: does not account for col and row, as it's not really important
        constexpr bool operator==(const Token &o) const
        {
            return IsType(o.type) && value == o.value;
        }

        friend std::ostream& operator<<(std::ostream& os, const Token& token) {
            os << "Token( " << magic_enum::enum_name(token.type) << " value='" << token.value << "' )";
            return os;
        }

        template <TokenType type>
        static constexpr Token Of()
        {
            return Token(type, GetDefaultTokenValue<type>());
        }

        template <TokenType type>
        static constexpr Token Of(const std::string_view& value)
        {
            return Token(type, value);
        }
    };

    // tokens where we know its value upfront, aka. the easy ones
    #define STATIC_TOKEN(X, Y) Token(TokenType::X, Y)
    constexpr std::array<Token, 17> StaticTokens = {
        STATIC_TOKEN(FN, "fn"),
        STATIC_TOKEN(PAREN_OPEN, "("),
        STATIC_TOKEN(PAREN_CLOSE, ")"),
        STATIC_TOKEN(COMMA, ","),
        STATIC_TOKEN(BRACE_OPEN, "{"),
        STATIC_TOKEN(BRACE_CLOSE, "}"),
        STATIC_TOKEN(FOR, "for"),
        STATIC_TOKEN(EQUALS, "="),
        STATIC_TOKEN(PLUS, "+"),
        STATIC_TOKEN(MINUS, "-"),
        STATIC_TOKEN(MULTIPLY, "*"),
        STATIC_TOKEN(DIVIDE, "/"),
        STATIC_TOKEN(SEMICOLON, ";"),
        STATIC_TOKEN(LESS_THAN, "<"),
        STATIC_TOKEN(GREATER_THAN, ">"),
        STATIC_TOKEN(RETURN, "return"),
        STATIC_TOKEN(MUT, "mut"),
    };
    #undef STATIC_TOKEN

    template <TokenType type>
    constexpr bool HasDefaultTokenValue() {
        for (const Token& token : StaticTokens) {
            if (token.type == type) {
                return true;
            }
        }
        return false;
    }

    template <TokenType type>
    constexpr StackString GetDefaultTokenValue() {
        static_assert(HasDefaultTokenValue<type>(), "Token type has no default value");
        for (const Token& token : StaticTokens) {
            if (token.type == type) {
                return token.value;
            }
        }
        throw std::logic_error("Token type has no default value");
    }

    constexpr const std::array TypeNames = {
        "mono",
        "int",
        "float",
        "bool",
        "string"
    };

    std::vector<Token> LexText(const std::string_view &text);
};