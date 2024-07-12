#pragma once

#include <string>

struct Token {
    std::string text;

    Token(std::string text);
    virtual ~Token() = default;

    friend std::ostream &operator<<(std::ostream &o, const Token &t);
    virtual void print(std::ostream &o) const;
};
