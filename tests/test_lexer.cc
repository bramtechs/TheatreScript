#include <span>
#include <gtest/gtest.h>
#include "theatre/lexer.hh"

using namespace theatre;

template <class T>
static void AssertArrays(const T& actual, const T& expected)
{
    ASSERT_EQ(actual.size(), expected.size());

    for (auto i = 0; i < actual.size(); i++) {
        if (actual[i] != expected[i]) {
            std::cerr << "Could not match " << expected[i] << " with " << actual[i] << '\n';
            FAIL();
        }
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
        Token::Of<TokenType::BRACE_OPEN>(),
        Token::Of<TokenType::BRACE_CLOSE>(),
        Token::Of<TokenType::PAREN_OPEN>(),
        Token::Of<TokenType::PAREN_CLOSE>(),
        Token::Of<TokenType::LESS_THAN>(),
        Token::Of<TokenType::GREATER_THAN>(),
        Token::Of<TokenType::COMMA>(),
        Token::Of<TokenType::EQUALS>(),
        Token::Of<TokenType::PLUS>(),
        Token::Of<TokenType::MINUS>(),
        Token::Of<TokenType::DIVIDE>(),
        Token::Of<TokenType::SEMICOLON>(),
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
        Token::Of<TokenType::FN>(),
        Token::Of<TokenType::IDENT>("takeSum"),
        Token::Of<TokenType::PAREN_OPEN>(),
        Token::Of<TokenType::TYPE>("int"),
        Token::Of<TokenType::LITERAL>("a"),
        Token::Of<TokenType::COMMA>(),
        Token::Of<TokenType::TYPE>("int"),
        Token::Of<TokenType::LITERAL>("b"),
        Token::Of<TokenType::PAREN_CLOSE>(),
        Token::Of<TokenType::TYPE>("int"),
        Token::Of<TokenType::BRACE_OPEN>(),
        Token::Of<TokenType::RETURN>(),
        Token::Of<TokenType::LITERAL>("a"),
        Token::Of<TokenType::PLUS>(),
        Token::Of<TokenType::LITERAL>("b"),
        Token::Of<TokenType::SEMICOLON>(),
        Token::Of<TokenType::BRACE_CLOSE>(),
    };

    AssertArrays(tokens, expected);
}