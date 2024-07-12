#include "Token.hpp"
#include <iostream>
#include <string>

Token::Token(std::string text) : text(text) {}

std::ostream &operator<<(std::ostream &o, const Token &t)
{
    t.print(o);
    return o;
}

void Token::print(std::ostream &o) const {
    o << "Token(" << text << ")";
}
