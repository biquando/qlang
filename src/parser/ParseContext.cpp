#include "parser/ParseContext.hpp"
#include "parser/ParseException.hpp"
#include "parser/Token.hpp"
#include <memory>
#include <sstream>
#include <string>

using parser::ParseContext;
using parser::Token;

auto ParseContext::eatGeneric(bool tokenIsValid,
                              const std::string &expectedToken)
    -> std::unique_ptr<Token>
{
    if (tokenIsValid) {
        auto tok = std::move(token);
        token = nextToken();
        return tok;
    }

    if (!expectedToken.empty()) {
        error("Expected " + expectedToken);
    }
    else {
        error();
    }
    return nullptr;
}

auto ParseContext::eat(Token::Id t) -> std::unique_ptr<Token>
{
    return eatGeneric(token && token->id() == t,
                      "Token::Id=" + std::to_string(t));
}

auto ParseContext::eat(char c) -> std::unique_ptr<Token>
{
    return eatGeneric(token && token->text.size() == 1 && token->text[0] == c,
                      "'" + std::string(1, c) + "'");
}

auto ParseContext::eat(const std::string &s) -> std::unique_ptr<Token>
{
    return eatGeneric(token && token->text == s, "\"" + s + "\"");
}

void ParseContext::error(const std::string &msg) const
{
    std::stringstream ss;
    if (token) {
        ss << "Found " << *token;
    }
    else {
        ss << "Unexpected end of input";
    }

    if (!msg.empty()) {
        ss << ": " + msg;
    }
    throw ParseException(i, ss.str());
}

auto ParseContext::nextToken() -> std::unique_ptr<Token>
{
    if (i == tokens.size()) {
        return nullptr;
    }
    return std::move(tokens[i++]);
}
