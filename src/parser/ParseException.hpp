#pragma once

#include <exception>
#include <string>

namespace parser {

class ParseException : public std::exception {
  public:
    ParseException() : msg("Parse error") {}
    ParseException(unsigned i, std::string extra = "") : ParseException()
    {
        msg += " at token " + std::to_string(i);
        if (!extra.empty()) {
            msg += ": " + extra;
        }
    }
    virtual const char *what() const noexcept override { return msg.c_str(); }

  private:
    std::string msg;
};

} // namespace parser
