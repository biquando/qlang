#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Production.hpp"
#include "parser/Token.hpp"

using namespace std;
using namespace lexer;
using namespace parser;

TEST(TestCombined, IfElse)
{
    Lexer<Token> lexer;
    lexer.opts.ignoreWhitespace = true;
    lexer.addTokenType("if");
    lexer.addTokenType("else");
    lexer.addTokenType("s");
    lexer.addTokenType("{");
    lexer.addTokenType("}");

    Production g("g");
    Production g1("g1");
    Production s("s");
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

    std::stringstream ss;
    ss << "  if {s{ {}s}s }else {   ss }  ";
    vector<unique_ptr<Token>> tokens = lexer.tokenize(ss);
    std::cout << g.produce(tokens) << "\n";
}
