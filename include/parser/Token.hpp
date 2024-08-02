#pragma once

#include <ostream>
#include <string>
#include <utility>

namespace parser {

class Token {
  public:
    std::string text;

    Token(char ch) : Token(std::to_string(ch)) {}
    Token(std::string text) : text(std::move(text)) {}
    virtual ~Token() = default;

    /*============
     | Token Ids |
     ============*/
  public:
    using Id = int;
    virtual auto id() -> Id { return id<Token>(); }

    template<typename T>
    static auto id() -> Id
    {
        static Id tokenId = newTokenId();
        return tokenId;
    }

  private:
    static auto newTokenId() -> Id;

    /*===========
     | Printing |
     ===========*/
  public:
    friend auto operator<<(std::ostream &o, const Token &t) -> std::ostream &;

  private:
    virtual void print(std::ostream &o) const;
};

} // namespace parser
