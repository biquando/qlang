#pragma once

#include <functional>
#include <istream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace lexer {

template<typename Token>
class Lexer {
  public:
    using Transition = std::function<int(int, char)>;
    using Constructor =
        std::function<std::unique_ptr<Token>(const std::string &)>;

    struct {
        bool ignoreWhitespace = false;
    } opts;

    Lexer() = default;

    void addTokenType(const Transition &transitionFn,
                      const Constructor &constructorFn);
    void addTokenType(const std::string &regex,
                      const Constructor &constructorFn);
    template<typename SubToken>
    void addTokenType(const Transition &transitionFn);
    template<typename SubToken>
    void addTokenType(const std::string &regex);
    void addTokenType(const std::string &regex);

    auto tokenize(std::istream &is) -> std::vector<std::unique_ptr<Token>>;

  private:
    auto nextChar(std::istream &is) -> int;
    auto transitionStates(std::vector<int> &states,
                          char c) -> std::pair<bool, int>;
    void handleOptions();

    std::vector<std::function<int(int, char)>> transitionFns;
    std::vector<std::function<std::unique_ptr<Token>(std::string)>>
        constructorFns;
    struct Location {
        unsigned long line = 1;
        unsigned long col = 0;
    } loc;
};

} // namespace lexer

#include "lexer/Lexer.tpp" // IWYU pragma: keep
