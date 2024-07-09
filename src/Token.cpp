#include "Token.hpp"
#include <string>

Token::Token(std::string text) : text(text) {}

std::ostream &operator<<(std::ostream &o, const Token &t)
{
    t.print(o);
    return o;
}
