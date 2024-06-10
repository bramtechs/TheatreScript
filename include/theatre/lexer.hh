#pragma once
#include <string_view>
#include <vector>
#include <string>

namespace theatre {

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

struct Token
{
    TokenType type;
    std::string value;
    int col;
    int row;

    Token(TokenType type, std::string value = "")
        : type(type), value(value)
    {
    }

    Token& At(int col, int row) {
        this->col = col;
        this->row = row;
        return *this;
    }

    inline constexpr bool IsType(TokenType type) const {
        return this->type == type;
    }

    // NOTE: does not account for col and row, as it's not really important
    constexpr bool operator ==(const Token& o) const {
        return IsType(o.type) && value == o.value;
    }
};

std::vector<Token> LexText(const std::string_view& text);

};