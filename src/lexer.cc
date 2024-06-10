#include "theatre/lexer.hh"
#include <unordered_map>
#include <optional>
#include <iostream>

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
    if (text.empty()) {
        return tokens;
    }

    for (auto i = 0; i < text.size(); i++) {
        const auto length = std::min(MAX_TOKEN_LEN, text.size() - i);
        const std::string_view view(text.data() + i, length);

        // skip whitespace
        if (std::isspace(view.front())) {
            continue;
        }

        std::optional<Token> found = std::nullopt;

        // find static tokens
        {
            std::string_view subView(view);

            while (!subView.empty()) {
                for (Token tok : StaticTokens) {
                    if (subView == tok.value) {
                        tokens.emplace_back(tok);
                        std::cout << "Found token: " << tok.value.data() << '\n';
                        found.emplace(tok);
                        break;
                    }
                }
                subView = std::string_view(subView.data(), subView.size() - 1);
            }
        }

        // TODO: find types
        // TODO: find literals

        // find identifiers
        if (!found)
        {
            // find next non alpha character
            for (auto j = 0; j < length; j++) {
                if (!std::isalpha(view[j])) {
                    Token identToken(TokenType::IDENT, view.substr(0, j));
                    std::cout << "Found identifier token: '" << identToken.value.data() << "'\n";
                    tokens.emplace_back(identToken);
                    found.emplace(identToken);
                    break;
                }
            }
        }

        if (!found) {
            throw LexerError("Didn't find anything?!");
        }

        auto skip = found->value.Length();
        i += skip;
    }

   return tokens;
}

std::vector<Token> LexText(const std::string_view& text)
{
    std::vector<Token> tokens{};
    return Lex(tokens, text);
}

};