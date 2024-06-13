#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

#include "theatre/parser.hh"

using namespace theatre;

TEST(ParserTests, ParseFunction) {
    const std::vector<Token> tokens = LexText(R"(
        fn takeSum(int a, int b) int {
            return a + b;
        }
    )");

    const auto tree = ParseTokens(tokens);

    ASSERT_TRUE(true);
}