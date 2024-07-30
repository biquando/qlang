#pragma once

#include "ParseContext.hpp"
#include "Token.hpp"
#include <iostream>
#include <memory>
#include <string>

std::unique_ptr<Token> ParseContext::eat(Token::Id t)
{
    if (token && token->id() == t) {
        auto tok = std::move(token);
        token = nextToken();
        return tok;
    }
    else {
        error();
        return nullptr;
    }
}

std::unique_ptr<Token> ParseContext::eat(char c)
{
    if (token && token->text.size() == 1 && token->text[0] == c) {
        auto tok = std::move(token);
        token = nextToken();
        return tok;
    }
    else {
        error();
        return nullptr;
    }
}

std::unique_ptr<Token> ParseContext::eat(std::string s)
{
    if (token && token->text == s) {
        auto tok = std::move(token);
        token = nextToken();
        return tok;
    }
    else {
        error();
        return nullptr;
    }
}

void ParseContext::error()
{
    // TODO: proper error handling
    std::cerr << "Parse error at token " << i << ": ";
    if (token) {
        // TODO: maybe we can show the expected token if possible?
        std::cerr << '\"' << token->text << '\"';
    }
    else {
        std::cerr << "Expected more tokens";
    }
    std::cerr << "\n";
    std::exit(1);
}

std::unique_ptr<Token> ParseContext::nextToken()
{
    if (i == tokens.size()) {
        return nullptr;
    }
    return std::move(tokens[i++]);
}

// vim:ft=cpp
