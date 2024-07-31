#include "parser/ParseContext.hpp"
#include "parser/ParseException.hpp"
#include "parser/Token.hpp"
#include <memory>
#include <sstream>
#include <string>

using parser::ParseContext;
using parser::Token;

std::unique_ptr<Token> ParseContext::eatGeneric(bool tokenIsValid,
                                                std::string expectedToken)
{
    if (tokenIsValid) {
        auto tok = std::move(token);
        token = nextToken();
        return tok;
    }
    else {
        if (!expectedToken.empty()) {
            error("Expected " + expectedToken);
            return nullptr;
        }
        error();
        return nullptr;
    }
}

std::unique_ptr<Token> ParseContext::eat(Token::Id t)
{
    return eatGeneric(token && token->id() == t,
                      "Token::Id=" + std::to_string(t));
}

std::unique_ptr<Token> ParseContext::eat(char c)
{
    return eatGeneric(token && token->text.size() == 1 && token->text[0] == c,
                      "'" + std::string(1, c) + "'");
}

std::unique_ptr<Token> ParseContext::eat(std::string s)
{
    return eatGeneric(token && token->text == s, "\"" + s + "\"");
}

void ParseContext::error(std::string msg)
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

std::unique_ptr<Token> ParseContext::nextToken()
{
    if (i == tokens.size()) {
        return nullptr;
    }
    return std::move(tokens[i++]);
}
