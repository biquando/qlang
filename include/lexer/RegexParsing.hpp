#pragma once

#include "lexer/Node.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace RegexParsing {

extern bool debug;

std::vector<int> tokenize(const std::string text);
bool validate(const std::vector<int> &tokens);

std::string tokensToString(const std::vector<int> &tokens);

struct Pattern {
    enum Type {
        Char,
        CharChoice,
        Concat,
        Alternate,
        Plus,
        Star,
        Optional
    } type;

    char literalChar = 0;
    std::function<bool(char)> charChoicePred;
    std::shared_ptr<Pattern> opr1;
    std::shared_ptr<Pattern> opr2;

    Pattern() = default;
    Pattern(std::string text);
    Pattern(std::vector<int> tokens);
};

std::unique_ptr<lexer::Node> toNode(std::shared_ptr<Pattern> p);
std::unique_ptr<lexer::Node> toNode(const std::string text);

} // namespace RegexParsing
