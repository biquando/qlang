#include "parser/Token.hpp"

#include <ostream>

using parser::Token;

auto Token::newTokenId() -> Token::Id
{
    static Id tokenId = 0;
    return ++tokenId;
}

auto parser::operator<<(std::ostream &o, const Token &t) -> std::ostream &
{
    t.print(o);
    return o;
}

void Token::print(std::ostream &o) const
{
    o << "Token(" << text << ")";
}
