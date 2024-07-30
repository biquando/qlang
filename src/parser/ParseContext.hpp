#pragma once

#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

namespace parser {

class ParseContext {
  public:
    std::unique_ptr<Token> token;
    unsigned i;

    ParseContext(std::vector<std::unique_ptr<Token>> &tokens) : tokens(tokens)
    {
        i = 0;
        token = nextToken();
    }
    std::unique_ptr<Token> eat(Token::Id t);
    std::unique_ptr<Token> eat(char c);
    std::unique_ptr<Token> eat(std::string s);
    void error(std::string msg = "");

  private:
    std::vector<std::unique_ptr<Token>> &tokens;
    std::unique_ptr<Token> eatGeneric(bool validToken,
                                      std::string expectedToken = "");
    std::unique_ptr<Token> nextToken();
};

} // namespace parser
