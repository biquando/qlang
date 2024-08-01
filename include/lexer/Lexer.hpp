#pragma once

#include <functional>
#include <istream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace lexer {

template <typename Token>
class Lexer {
  public:
    Lexer() = default;

    struct {
        bool ignoreWhitespace = false;
    } opts;

    void addTokenType(
        const std::function<int(int, char)> &transitionFn,
        const std::function<std::unique_ptr<Token>(const std::string &)>
            &constructorFn);

    void addTokenType(
        const std::string &regex,
        const std::function<std::unique_ptr<Token>(const std::string &)>
            &constructorFn);

    template <typename SubToken>
    void addTokenType(const std::function<int(int, char)> &transitionFn);

    template <typename SubToken>
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
