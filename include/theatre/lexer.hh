#pragma once
#include <string_view>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>

namespace theatre
{
    using LexerError = std::runtime_error;

    enum class TokenType : int
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
    };

    struct Token
    {
        const TokenType type;
        const StackString value;
        int col{-1};
        int row{-1};

        constexpr Token(TokenType type, const std::string_view &value = "")
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

        // NOTE: does not account for col and row, as it's not really important
        constexpr bool operator==(const Token &o) const
        {
            return IsType(o.type) && value == o.value;
        }
    };

    // tokens where we know its value upfront, aka. the easy ones
    #define STATIC_TOKEN(X, Y) Token(TokenType::X, Y)
    constexpr std::array<Token, 17> StaticTokens = {
        STATIC_TOKEN(FN, "fn"),
        STATIC_TOKEN(PAREN_OPEN, "{"),
        STATIC_TOKEN(PAREN_OPEN, "}"),
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

    constexpr const std::array TypeNames = {
        "mono",
        "int",
        "float",
        "bool",
        "string"
    };
    
    std::vector<Token> LexText(const std::string_view &text);
};