#include <gtest/gtest.h>
#include "theatre/lexer.hh"

using namespace theatre;

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
    
    ASSERT_EQ(tokens.size(), expected.size());

    for (auto i = 0; i < tokens.size(); i++) {
        ASSERT_EQ(expected[i], tokens[i]);
    }
}