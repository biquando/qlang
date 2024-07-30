#include "Token.hpp"

using parser::Token;

Token::Id Token::newTokenId()
{
    static Id tokenId = 0;
    return ++tokenId;
}

std::ostream &parser::operator<<(std::ostream &o, const Token &t)
{
    t.print(o);
    return o;
}

void Token::print(std::ostream &o) const { o << "Token(" << text << ")"; }
