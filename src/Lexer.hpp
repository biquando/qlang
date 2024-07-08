#pragma once

#include <functional>
#include <vector>
#include "Token.hpp"

class Lexer {
public:
    Lexer() = default;
    std::vector<std::unique_ptr<Token>> tokenize(FILE *fd);

    void addTokenType(
        std::function<int(int, char)> transitionFn,
        std::function<std::unique_ptr<Token>(std::string)> constructorFn
    ) {
        transitionFns.push_back(transitionFn);
        constructorFns.push_back(constructorFn);
    }

    int numTokenTypes() const {
        return transitionFns.size();
    }

private:
    int nextChar(FILE *fd);
    std::pair<bool, int> transitionStates(std::vector<int> &states, char c);

    std::vector<std::function<int(int, char)>> transitionFns;
    std::vector<std::function<std::unique_ptr<Token>(std::string)>> constructorFns;
    struct Location {
        unsigned long line = 1;
        unsigned long col = 0;
    } loc;
};
