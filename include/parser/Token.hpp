#pragma once

#include <ostream>
#include <string>

namespace parser {

class Token {
  public:
    std::string text;

    Token(char ch) : Token(std::to_string(ch)) {}
    Token(std::string text) : text(text) {}
    virtual ~Token() = default;

    /*============
     | Token Ids |
     ============*/
  public:
    typedef int Id;
    virtual Id id() { return id<Token>(); }

    template <typename T>
    static Id id()
    {
        static Id tokenId = newTokenId();
        return tokenId;
    }

  private:
    static Id newTokenId();

    /*===========
     | Printing |
     ===========*/
  public:
    friend std::ostream &operator<<(std::ostream &o, const Token &t);

  private:
    virtual void print(std::ostream &o) const;
};

} // namespace parser
