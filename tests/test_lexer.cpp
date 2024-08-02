#include <cctype>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "lexer/Lexer.hpp"

using namespace lexer;

struct Token {
    std::string text;

    Token(std::string text) : text(std::move(text)) {}
    virtual ~Token() = default;

    friend auto operator<<(std::ostream &o, const Token &t) -> std::ostream &
    {
        t.print(o);
        return o;
    }
    virtual void print(std::ostream &o) const { o << "Token(" << text << ")"; }
};

struct DecimalToken : Token {
    long val;
    DecimalToken(const std::string &text) : Token(text), val(std::stol(text)) {}
    void print(std::ostream &o) const override
    {
        o << "DecimalToken(" << val << ")";
    }
};

struct HexToken : Token {
    unsigned long val;
    HexToken(const std::string &text)
        : Token(text),
          val(std::stol(text.substr(2), nullptr, 16))
    {}
    void print(std::ostream &o) const override
    {
        o << "HexToken(" << val << ")";
    }
};

TEST(TestLexer, Literals)
{
    Lexer<Token> l;
    l.opts.ignoreWhitespace = true;
    l.addTokenType<DecimalToken>(R"( [0-9]+ )");
    l.addTokenType<HexToken>(R"( 0x[0-9a-fA-F]+ )");
    l.addTokenType(R"( \"([^"\\\n]|\\.)*\" )");
    l.addTokenType(R"( \'([^'\\\n]|\\.)\' )");
    l.addTokenType(R"(\/\/.*)", [](const std::string &) { return nullptr; });
    l.addTokenType(R"(\/\*[^*]*\*+([^/*][^*]*\*+)*\/)",
                   [](const std::string &) { return nullptr; });

    std::stringstream ss;
    ss << "   1234     0x20\"hello world\\\"\"\n";
    ss << "    999'\\\\'0xff\"\"\n";
    ss << "// hello world asdf 123\n";
    ss << "/* siuofeh iusefh isuefh\n";
    ss << "siuehf iuseh */\n";
    std::vector<std::unique_ptr<Token>> tokens = l.tokenize(ss);
    std::cout << "\n=== TOKENS (" << tokens.size() << ") ===\n";
    for (auto &token : tokens) {
        std::cout << *token << "\n";
    }
}
