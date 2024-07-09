#include "Lexer.hpp"
#include "State.hpp"
#include "Token.hpp"
#include <iostream>

/*=================
 | State machines |
 =================*/

// Decimal:    [0-9]+
// Hex:        0x[0-9a-fA-F]+
// Whitespace: [ ]+

enum DecimalState {
    DecimalEnter = (int)State::Enter, // [0-9]
    Decimal1 = 1,                     // [0-9]*
    DecimalAccept = (int)State::Accept,
    DecimalReject = (int)State::Reject,
};

enum HexState {
    HexEnter = (int)State::Enter, // 0
    Hex1 = 1,                     // x
    Hex2,                         // [0-9a-fA-F]
    Hex3,                         // [0-9a-fA-F]*
    HexAccept = (int)State::Accept,
    HexReject = (int)State::Reject,
};

enum WhitespaceState {
    WhitespaceEnter = (int)State::Enter,
    Whitespace1 = 1,
    WhitespaceAccept = (int)State::Accept,
    WhitespaceReject = (int)State::Reject,
};

DecimalState transitionDecimal(DecimalState currState, char c)
{
    switch (currState) {
    case DecimalEnter:
        if (isnumber(c)) {
            return Decimal1;
        }
        return DecimalReject;
    case Decimal1:
        if (isnumber(c)) {
            return Decimal1;
        }
        return DecimalAccept;
    case DecimalAccept:
        return DecimalReject;
    case DecimalReject:
        return DecimalReject;
    }
}

HexState transitionHex(HexState currState, char c)
{
    switch (currState) {
    case HexEnter:
        if (c == '0') {
            return Hex1;
        }
        return HexReject;
    case Hex1:
        if (c == 'x') {
            return Hex2;
        }
        return HexReject;
    case Hex2:
        if (ishexnumber(c)) {
            return Hex3;
        }
        return HexReject;
    case Hex3:
        if (ishexnumber(c)) {
            return Hex3;
        }
        return HexAccept;
    case HexAccept:
        return HexReject;
    case HexReject:
        return HexReject;
    }
}

WhitespaceState transitionWhitespace(WhitespaceState currState, char c)
{
    switch (currState) {
    case WhitespaceEnter:
        if (isspace(c)) {
            return Whitespace1;
        }
        return WhitespaceReject;
    case Whitespace1:
        if (isspace(c)) {
            return Whitespace1;
        }
        return WhitespaceAccept;
    case WhitespaceAccept:
        return WhitespaceReject;
    case WhitespaceReject:
        return WhitespaceReject;
    }
}

/*====================
 | Token definitions |
 ====================*/

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

int main(void)
{
    Lexer l;
    l.addTokenType(
        [](int s, char c) { return transitionDecimal((DecimalState)s, c); },
        [](std::string text) { return std::make_unique<DecimalToken>(text); });
    l.addTokenType(
        [](int s, char c) { return transitionHex((HexState)s, c); },
        [](std::string text) { return std::make_unique<HexToken>(text); });
    l.addTokenType(
        [](int s, char c) {
            return transitionWhitespace((WhitespaceState)s, c);
        },
        [](std::string) { return std::unique_ptr<Token>(); });

    std::vector<std::unique_ptr<Token>> tokens = l.tokenize(stdin);
    std::cout << "=== TOKENS (" << tokens.size() << ") ===\n";
    for (auto &token : tokens) {
        std::cout << *token << "\n";
    }
}
