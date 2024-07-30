#include "lexer/Lexer.hpp"
#include <cctype>
#include <iostream>
#include <memory>
#include <string>

using namespace lexer;

struct Token {
    std::string text;

    Token(std::string text) : text(text) {}
    virtual ~Token() = default;

    friend std::ostream &operator<<(std::ostream &o, const Token &t)
    {
        t.print(o);
        return o;
    }
    virtual void print(std::ostream &o) const { o << "Token(" << text << ")"; }
};

struct DecimalToken : Token {
    long val;
    DecimalToken(std::string text) : Token(text), val(std::stol(text)) {}
    void print(std::ostream &o) const override
    {
        o << "DecimalToken(" << val << ")";
    }
};

struct HexToken : Token {
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
    Lexer<Token> l;
    l.opts.ignoreWhitespace = true;
    l.addTokenType<DecimalToken>(R"( [0-9]+ )");
    l.addTokenType<HexToken>(R"( 0x[0-9a-fA-F]+ )");
    l.addTokenType(R"( \"([^"\\\n]|\\.)*\" )");
    l.addTokenType(R"( \'([^'\\\n]|\\.)\' )");
    l.addTokenType(R"(\/\/.*)", [](std::string) { return nullptr; });
    l.addTokenType(R"(\/\*[^*]*\*+([^/*][^*]*\*+)*\/)",
                   [](std::string) { return nullptr; });

    std::vector<std::unique_ptr<Token>> tokens = l.tokenize(stdin);
    std::cout << "\n=== TOKENS (" << tokens.size() << ") ===\n";
    for (auto &token : tokens) {
        std::cout << *token << "\n";
    }
}
