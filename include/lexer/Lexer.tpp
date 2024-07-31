#pragma once

#include "lexer/LexException.hpp"
#include "lexer/Lexer.hpp"
#include "lexer/RegexParsing.hpp"
#include "lexer/State.hpp"
#include "lexer/StateMachine.hpp"
#include <functional>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

template <typename Token>
void lexer::Lexer<Token>::addTokenType(
    std::function<int(int, char)> transitionFn,
    std::function<std::unique_ptr<Token>(std::string)> constructorFn)
{
    transitionFns.push_back(transitionFn);
    constructorFns.push_back(constructorFn);
}

template <typename Token>
void lexer::Lexer<Token>::addTokenType(
    std::string regex,
    std::function<std::unique_ptr<Token>(std::string)> constructorFn)
{
    StateMachine sm(RegexParsing::toNode(regex));
    transitionFns.push_back(
        [sm](int s, char c) { return sm.transition(s, c); });
    constructorFns.push_back(constructorFn);
}

template <typename Token>
template <typename SubToken>
void lexer::Lexer<Token>::addTokenType(
    std::function<int(int, char)> transitionFn)
{
    transitionFns.push_back(transitionFn);
    constructorFns.push_back(
        [](std::string text) { return std::make_unique<SubToken>(text); });
}

template <typename Token>
template <typename SubToken>
void lexer::Lexer<Token>::addTokenType(std::string regex)
{
    StateMachine sm(RegexParsing::toNode(regex));
    transitionFns.push_back(
        [sm](int s, char c) { return sm.transition(s, c); });
    constructorFns.push_back(
        [](std::string text) { return std::make_unique<SubToken>(text); });
}

template <typename Token>
void lexer::Lexer<Token>::addTokenType(std::string regex)
{
    addTokenType<Token>(regex);
}

template <typename Token>
int lexer::Lexer<Token>::nextChar(std::istream &is)
{
    if (is.eof()) {
        return EOF;
    }

    char c = is.get();
    if (c == '\0') {
        return EOF;
    }
    else if (c == '\n') {
        loc.line++;
        loc.col = 0;
    }
    else {
        loc.col++;
    }
    return c;
}

template <typename Token>
std::pair<bool, int>
lexer::Lexer<Token>::transitionStates(std::vector<int> &states, char c)
{
    bool stillMatching = false;
    int firstAcceptedState = -1;

    for (int i = 0; i < (int)states.size(); i++) {
        states[i] = transitionFns[i](states[i], c);
        if (firstAcceptedState < 0 && states[i] == (int)State::Accept) {
            firstAcceptedState = i;
        }
        if (states[i] != (int)State::Accept &&
            states[i] != (int)State::Reject) {
            stillMatching = true;
        }
    }

    return std::pair<bool, int>(stillMatching, firstAcceptedState);
}

template <typename Token>
void lexer::Lexer<Token>::handleOptions()
{
    if (opts.ignoreWhitespace) {
        StateMachine ws(RegexParsing::toNode(R"([ \r\n\t\v]+)"));
        addTokenType([ws](int s, char c) { return ws.transition(s, c); },
                     [](std::string) { return nullptr; });
    }
}

template <typename Token>
std::vector<std::unique_ptr<Token>>
lexer::Lexer<Token>::tokenize(std::istream &is)
{
    handleOptions();
    std::vector<std::unique_ptr<Token>> tokens;
    std::vector<char> currToken;
    std::vector<int> states(transitionFns.size(), (int)State::Enter);

    std::function<void()> reset = [&currToken, &states]() {
        currToken.clear();
        for (int &state : states) {
            state = (int)State::Enter;
        }
    };
    reset();

    int c = nextChar(is);
    if (c == EOF) {
        return tokens;
    }
    while (true) {
        auto [stillMatching, firstAcceptedState] = transitionStates(states, c);

        if (!stillMatching) {
            if (firstAcceptedState < 0) {
                std::stringstream ss;
                ss << "Unexpected character ";
                if (c == EOF) {
                    ss << "EOF";
                }
                else if (isprint(c)) {
                    ss << "`" << (char)c << "`";
                }
                else {
                    ss << "0x" << std::hex << (int)c << std::dec;
                }
                throw lexer::LexException(loc.line, loc.col, ss.str());
                return tokens;
            }
            else {
                std::string tokenText(currToken.begin(), currToken.end());
                std::unique_ptr<Token> token =
                    constructorFns[firstAcceptedState](tokenText);
                if (token != nullptr) {
                    tokens.push_back(std::move(token));
                }
            }
            reset();

            if (c == EOF) {
                break;
            }
            continue;
        }
        if (c == EOF) {
            std::cerr << "Unexpected EOF\n";
            throw lexer::LexException(loc.line, loc.col, "Unexpected EOF");
            break;
        }

        currToken.push_back(c);
        c = nextChar(is);
    }

    return tokens;
}

// vim:ft=cpp