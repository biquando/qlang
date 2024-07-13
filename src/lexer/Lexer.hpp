#pragma once

#include <functional>
#include <string>
#include <vector>

template <typename Token>
class Lexer {
  public:
    Lexer() = default;

    struct {
        bool ignoreWhitespace = false;
    } opts;

    void addTokenType(
        std::function<int(int, char)> transitionFn,
        std::function<std::unique_ptr<Token>(std::string)> constructorFn);

    template <typename SubToken>
    void addTokenType(std::function<int(int, char)> transitionFn);

    template <typename SubToken>
    void addTokenType(std::string regex);

    void addTokenType(std::string regex);

    std::vector<std::unique_ptr<Token>> tokenize(FILE *fd);

  private:
    int nextChar(FILE *fd);
    std::pair<bool, int> transitionStates(std::vector<int> &states, char c);
    void handleOptions();

    std::vector<std::function<int(int, char)>> transitionFns;
    std::vector<std::function<std::unique_ptr<Token>(std::string)>>
        constructorFns;
    struct Location {
        unsigned long line = 1;
        unsigned long col = 0;
    } loc;
};

#include "Lexer.tpp"
