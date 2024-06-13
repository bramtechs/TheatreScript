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

        {
            std::string_view subView(view);

            while (!subView.empty()) {
                // find static tokens
                for (Token tok : StaticTokens) {
                    if (subView == tok.value) {
                        tokens.emplace_back(tok);
                        std::cout << "Found token: " << tok << '\n';
                        found.emplace(tok);
                        break;
                    }
                }

                // find types
                for (const std::string& type : TypeNames)
                {
                    if (subView == type) {
                        Token tok(TokenType::TYPE, type);
                        tokens.emplace_back(tok);
                        std::cout << "Found type: " << tok << '\n';
                        found.emplace(tok);
                        break;
                    }
                }

                subView = std::string_view(subView.data(), subView.size() - 1);
            }
        }

        // TODO: find literals

        // find identifiers
        if (!found.has_value())
        {
            // find next non alpha character
            for (auto j = 0; j < length; j++) {
                if (!std::isalpha(view[j])) {
                    Token identToken(TokenType::IDENT, view.substr(0, j)); // TODO fix: idents can have empty value
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

        int skip = static_cast<int>(found->value.Length());
        i += std::max(0, skip - 1);
    }

   return tokens;
}

std::vector<Token> LexText(const std::string_view& text)
{
    std::vector<Token> tokens{};
    return Lex(tokens, text);
}

};