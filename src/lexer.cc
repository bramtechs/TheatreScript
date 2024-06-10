#include "theatre/lexer.hh"
#include <unordered_map>
#include <optional>

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

static std::vector<Token> Lex(std::vector<Token>& tokens, const std::string_view& text)
{
    /*
    if (text.empty()) {
        return tokens;
    }

    for (auto i = 0; i < text.size(); i++) {
        // skip whitespace
        if (std::isspace(*it)) {
            continue;
        }

        std::string_view view(it, MAX_TOKEN_LEN);
        while (!view.empty()) {
            for (const Token& tok : StaticTokens) {
                if (view = tok.value) {
                    tokens.emplace(tok);
                    break;
                }
            }
        }
    }
    */

   return tokens;
}

std::vector<Token> LexText(const std::string_view& text)
{
    std::vector<Token> tokens{};
    return Lex(tokens, text);
}

};