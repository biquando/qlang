#include "lexer/Lexer.hpp"
#include "lexer/RegexParsing.hpp"
#include "lexer/StateMachine.hpp"
#include "lexer/Token.hpp"
#include <cctype>
#include <iostream>
#include <memory>

struct DecimalToken : public Token {
    long val;
    DecimalToken(std::string text) : Token(text), val(std::stol(text)) {}
    void print(std::ostream &o) const override
    {
        o << "DecimalToken(" << val << ")";
    }
};

struct HexToken : public Token {
    long val;
    HexToken(std::string text) : Token(text)
    {
        if (text[0] == '+') {
            val = std::stol(text.substr(3), nullptr, 16);
        }
        else if (text[0] == '-') {
            val = -std::stol(text.substr(3), nullptr, 16);
        }
        else {
            val = std::stol(text.substr(2), nullptr, 16);
        }
    }
    void print(std::ostream &o) const override
    {
        o << "HexToken(" << val << ")";
    }
};

// Decimal:    [0-9]+
// Hex:        0x[0-9a-fA-F]+
// String:     \"([^"\\\n]|\\.)*\"

int main(void)
{
    // RegexParsing::debug = true;
    StateMachine dec(RegexParsing::toNode(R"( [+\-]?[0-9]+ )"));
    StateMachine hex(RegexParsing::toNode(R"( [+\-]?0x[0-9a-fA-F]+ )"));
    StateMachine str(RegexParsing::toNode(R"( \"([^"\\\n]|\\.)*\" )"));

    Lexer l;
    l.opts.ignoreWhitespace = true;
    l.addTokenType(
        [&dec](int s, char c) { return dec.transition(s, c); },
        [](std::string text) { return std::make_unique<DecimalToken>(text); });
    l.addTokenType(
        [&hex](int s, char c) { return hex.transition(s, c); },
        [](std::string text) { return std::make_unique<HexToken>(text); });
    l.addTokenType(
        [&str](int s, char c) { return str.transition(s, c); },
        [](std::string text) { return std::make_unique<Token>(text); });

    std::vector<std::unique_ptr<Token>> tokens = l.tokenize(stdin);
    std::cout << "\n=== TOKENS (" << tokens.size() << ") ===\n";
    for (auto &token : tokens) {
        std::cout << *token << "\n";
    }
}
