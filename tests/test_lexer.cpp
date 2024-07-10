#include "lexer/Lexer.hpp"
#include "lexer/Node.hpp"
#include "lexer/State.hpp"
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

struct StringToken : public Token {
    // TODO: parse string (handle quotes and escape characters)
    StringToken(std::string text) : Token(text) {}
    void print(std::ostream &o) const override
    {
        o << "StringToken(" << text << ")";
    }
};

// Decimal:    [0-9]+
// Hex:        0x[0-9a-fA-F]+
// String:     \"([^"\\\n]|\\.)*\"
// Whitespace: [ ]+

int main(void)
{
    // Construct state machines
    StateMachine dec(std::make_unique<PlusNode>(std::make_unique<LiteralNode>(
        std::make_unique<PredState>([](char c) { return isnumber(c); }))));
    StateMachine hex(std::make_unique<ConcatNode>(
        std::make_unique<ConcatNode>(
            std::make_unique<LiteralNode>(std::make_shared<CharState>('0')),
            std::make_unique<LiteralNode>(std::make_shared<CharState>('x'))),
        std::make_unique<PlusNode>(
            std::make_unique<LiteralNode>(std::make_shared<PredState>(
                [](char c) { return ishexnumber(c); })))));
    StateMachine str(std::make_unique<ConcatNode>(
        std::make_unique<ConcatNode>(
            std::make_unique<LiteralNode>(std::make_shared<CharState>('\"')),
            std::make_unique<StarNode>(std::make_unique<AlternateNode>(
                std::make_unique<LiteralNode>(
                    std::make_shared<PredState>([](char c) {
                        return c != EOF && c != '\"' && c != '\\' && c != '\n';
                    })),
                std::make_unique<ConcatNode>(
                    std::make_unique<LiteralNode>(
                        std::make_shared<CharState>('\\')),
                    std::make_unique<LiteralNode>(std::make_shared<PredState>(
                        [](char c) { return c != EOF && c != '\n'; })))))),
        std::make_unique<LiteralNode>(std::make_shared<CharState>('\"'))));
    StateMachine ws(std::make_unique<PlusNode>(std::make_unique<LiteralNode>(
        std::make_unique<PredState>([](char c) { return isspace(c); }))));

    Lexer l;
    l.addTokenType(
        [&dec](int s, char c) { return dec.transition(s, c); },
        [](std::string text) { return std::make_unique<DecimalToken>(text); });
    l.addTokenType(
        [&hex](int s, char c) { return hex.transition(s, c); },
        [](std::string text) { return std::make_unique<HexToken>(text); });
    l.addTokenType(
        [&str](int s, char c) { return str.transition(s, c); },
        [](std::string text) { return std::make_unique<StringToken>(text); });
    l.addTokenType([&ws](int s, char c) { return ws.transition(s, c); },
                   [](std::string) { return nullptr; });
    // l.addTokenType(
    //     [&test](int s, char c) { return test.transition(s, c); },
    //     [](std::string text) { return std::make_unique<StringToken>(text);
    //     });

    std::vector<std::unique_ptr<Token>> tokens = l.tokenize(stdin);
    std::cout << "=== TOKENS (" << tokens.size() << ") ===\n";
    for (auto &token : tokens) {
        std::cout << *token << "\n";
    }
}
