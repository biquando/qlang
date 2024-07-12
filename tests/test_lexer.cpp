#include "lexer/Lexer.hpp"
#include "lexer/RegexParsing.hpp"
#include "lexer/StateMachine.hpp"
#include "lexer/Token.hpp"
#include <cctype>
#include <iostream>
#include <memory>

struct DecimalToken : public Token {
    unsigned long val;
    DecimalToken(std::string text) : Token(text), val(std::stol(text)) {}
    void print(std::ostream &o) const override
    {
        o << "DecimalToken(" << val << ")";
    }
};

struct HexToken : public Token {
    unsigned long val;
    HexToken(std::string text)
        : Token(text), val(std::stol(text.substr(2), nullptr, 16))
    {
    }
    void print(std::ostream &o) const override
    {
        o << "HexToken(" << val << ")";
    }
};

// Decimal:    [0-9]+
// Hex:        0x[0-9a-fA-F]+
// String:     \"([^"\\\n]|\\.)*\"
// Whitespace: [ ]+

int main(void)
{
    RegexParsing::debug = false;
    StateMachine dec(RegexParsing::toNode(R"([0-9]+)"));
    StateMachine hex(RegexParsing::toNode(R"(0x[0-9a-fA-F]+)"));
    StateMachine str(RegexParsing::toNode(R"(\"([^"\\\n]|\\.)*\")"));
    StateMachine ws(RegexParsing::toNode(R"([ \r\n\t\v]+)"));

    Lexer l;
    l.addTokenType(
        [&dec](int s, char c) { return dec.transition(s, c); },
        [](std::string text) { return std::make_unique<DecimalToken>(text); });
    l.addTokenType(
        [&hex](int s, char c) { return hex.transition(s, c); },
        [](std::string text) { return std::make_unique<HexToken>(text); });
    l.addTokenType(
        [&str](int s, char c) { return str.transition(s, c); },
        [](std::string text) { return std::make_unique<Token>(text); });
    l.addTokenType([&ws](int s, char c) { return ws.transition(s, c); },
                   [](std::string) { return nullptr; });

    // StateMachine test(RegexParsing::toNode(R"(ab*c)"));
    // l.addTokenType(
    //     [&test](int s, char c) { return test.transition(s, c); },
    //     [](std::string text) { return std::make_unique<StringToken>(text); });

    std::vector<std::unique_ptr<Token>> tokens = l.tokenize(stdin);
    std::cout << "\n=== TOKENS (" << tokens.size() << ") ===\n";
    for (auto &token : tokens) {
        std::cout << *token << "\n";
    }
}
