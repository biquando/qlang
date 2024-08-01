#pragma once

#include "lexer/Node.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace RegexParsing {

extern bool debug;

auto tokenize(const std::string &text) -> std::vector<int>;
auto validate(const std::vector<int> &tokens) -> bool;

auto tokensToString(const std::vector<int> &tokens) -> std::string;

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
    Pattern(const std::string &text);
    Pattern(const std::vector<int> &tokens);
};

auto toNode(const std::shared_ptr<Pattern> &p) -> std::unique_ptr<lexer::Node>;
auto toNode(const std::string &text) -> std::unique_ptr<lexer::Node>;

} // namespace RegexParsing
