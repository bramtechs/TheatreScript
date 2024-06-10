#include "theatre/lexer.hh"

namespace theatre {


const char* script = R"(

fn takeSum(int a, int b) int {
    return a + b;
}

for (int i = 0; i < 2; i++)
{
    mut int offset = ( 2 + i ) + 5 * 0 - 1;
    offset += 1;
    int sum = takeSum(10, offset);
    if (sum > 15) {
        print("wow!!!");
    }
    else {
        print("aww...");
    }
}
)";

std::vector<Token> LexText(const std::string_view& text)
{
    return {};
}

};