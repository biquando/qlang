#pragma once

#include <exception>
#include <string>

namespace lexer {

class LexException : public std::exception {
  public:
    LexException() : msg("Lex error") {}
    LexException(unsigned long line, unsigned long col,
                 const std::string &extra = "")
        : LexException()
    {
        msg +=
            " at line " + std::to_string(line) + " col " + std::to_string(col);
        if (!extra.empty()) {
            msg += ": " + extra;
        }
    }
    auto what() const noexcept -> const char * override { return msg.c_str(); }

  private:
    std::string msg;
};

} // namespace lexer
