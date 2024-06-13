#include <span>
#include <gtest/gtest.h>
#include "theatre/lexer.hh"

using namespace theatre;

template <class T>
static void AssertArrays(const T& actual, const T& expected)
{
    ASSERT_EQ(actual.size(), expected.size());

    for (auto i = 0; i < actual.size(); i++) {
        ASSERT_EQ(actual[i], expected[i]);
    }
}

TEST(LexerTests, IsAlpha) {
    ASSERT_TRUE(std::isalpha('a'));
    ASSERT_TRUE(std::isalpha('z'));
    ASSERT_TRUE(std::isalpha('A'));
    ASSERT_TRUE(std::isalpha('Z'));
    ASSERT_FALSE(std::isalpha('('));
    ASSERT_FALSE(std::isalpha('&'));
    ASSERT_FALSE(std::isalpha('0'));
    ASSERT_FALSE(std::isalpha('9'));
}

TEST(LexerTests, LexSymbols) {
    const auto tokens = LexText(R"(
        {}()<>,=+-/;
    )");

    const std::vector<Token> expected = {
        Token(TokenType::BRACE_OPEN),
        Token(TokenType::BRACE_CLOSE),
        Token(TokenType::PAREN_OPEN),
        Token(TokenType::PAREN_CLOSE),
        Token(TokenType::LESS_THAN),
        Token(TokenType::GREATER_THAN),
        Token(TokenType::COMMA),
        Token(TokenType::EQUALS),
        Token(TokenType::PLUS),
        Token(TokenType::MINUS),
        Token(TokenType::DIVIDE),
        Token(TokenType::SEMICOLON),
    };

    AssertArrays(tokens, expected);
}

TEST(LexerTests, LexFnDeclaration) {
    const auto tokens = LexText(R"(
    
    fn takeSum(int a, int b) int {
        return a + b;
    }
    
    )");

    const std::vector<Token> expected = {
        Token(TokenType::FN),
        Token(TokenType::IDENT),
        Token(TokenType::PAREN_OPEN),
        Token(TokenType::TYPE),
        Token(TokenType::LITERAL),
        Token(TokenType::COMMA),
        Token(TokenType::TYPE),
        Token(TokenType::LITERAL),
        Token(TokenType::PAREN_CLOSE),
        Token(TokenType::TYPE),
        Token(TokenType::BRACE_OPEN),
        Token(TokenType::RETURN),
        Token(TokenType::LITERAL),
        Token(TokenType::PLUS),
        Token(TokenType::LITERAL),
        Token(TokenType::SEMICOLON),
        Token(TokenType::BRACE_CLOSE),
    };

    AssertArrays(tokens, expected);
}