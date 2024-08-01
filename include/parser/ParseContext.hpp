#pragma once

#include <memory>
#include <string>
#include <vector>

#include "parser/Token.hpp"

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
    auto eat(Token::Id t) -> std::unique_ptr<Token>;
    auto eat(char c) -> std::unique_ptr<Token>;
    auto eat(const std::string &s) -> std::unique_ptr<Token>;
    void error(const std::string &msg = "") const;

  private:
    std::vector<std::unique_ptr<Token>> &tokens;
    auto eatGeneric(bool tokenIsValid, const std::string &expectedToken = "")
        -> std::unique_ptr<Token>;
    auto nextToken() -> std::unique_ptr<Token>;
};

} // namespace parser
