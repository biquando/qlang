#include "parser/ParseContext.hpp"
#include "parser/Production.hpp"
#include "parser/Token.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

using namespace parser;

struct TextToken : public Token {
    TextToken(std::string text) : Token(text) {}
    Token::Id id() override { return Token::id<TextToken>(); }
};

void addToken(std::vector<std::unique_ptr<Token>> &tokens, std::string text)
{
    auto p = std::make_unique<TextToken>(text);
    tokens.push_back(std::move(p));
}

int main()
{
    // G  -> i S G1
    // G1 -> e S
    //    ->
    // S  -> s
    //    -> { S1
    // S1 -> L }
    //    -> }
    // L  -> S L1
    // L1 -> L
    //    ->
    Production g("g");
    Production g1("g1");
    Production s;
    Production s1("s1");
    Production l("l");
    Production l1("l1");
    g.add({"if", &s, &g1});
    g1.add({"else", &s});
    g1.add({});
    s.add({'s'});
    s.add({'{', &s1});
    s1.add({&l, '}'});
    s1.add({'}'});
    l.add({&s, &l1});
    l1.add({&l});
    l1.add({});

    // if{s{{}s}s}else{ss}
    std::vector<std::unique_ptr<Token>> tokens;
    addToken(tokens, "if");
    addToken(tokens, "{");
    addToken(tokens, "s");
    addToken(tokens, "{");
    addToken(tokens, "{");
    addToken(tokens, "}");
    addToken(tokens, "s");
    addToken(tokens, "}");
    addToken(tokens, "s");
    addToken(tokens, "}");
    addToken(tokens, "else");
    addToken(tokens, "{");
    addToken(tokens, "s");
    addToken(tokens, "s");
    addToken(tokens, "}");

    ParseContext ctx(tokens);

    Production::debug = true;
    Production::Node n = g.produce(ctx, true);

    std::cout << n << "\n";
}
