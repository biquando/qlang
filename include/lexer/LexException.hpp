#pragma once

#include <exception>
#include <string>

namespace lexer {

class LexException : public std::exception {
  public:
    LexException() : msg("Lex error") {}
    LexException(unsigned long line, unsigned long col, std::string extra = "")
        : LexException()
    {
        msg +=
            " at line " + std::to_string(line) + " col " + std::to_string(col);
        if (!extra.empty()) {
            msg += ": " + extra;
        }
    }
    virtual const char *what() const noexcept override { return msg.c_str(); }

  private:
    std::string msg;
};

} // namespace lexer
