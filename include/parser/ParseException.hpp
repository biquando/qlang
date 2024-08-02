#pragma once

#include <exception>
#include <string>

namespace parser {

class ParseException : public std::exception {
  public:
    ParseException() : msg("Parse error") {}
    ParseException(unsigned i, const std::string &extra = "") : ParseException()
    {
        msg += " at token " + std::to_string(i);
        if (!extra.empty()) {
            msg += ": " + extra;
        }
    }
    auto what() const noexcept -> const char * override { return msg.c_str(); }

  private:
    std::string msg;
};

} // namespace parser
