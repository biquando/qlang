#pragma once

#include <initializer_list>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "parser/ParseContext.hpp"
#include "parser/Token.hpp"

namespace parser {

struct Epsilon {};

struct Terminal {
    std::unique_ptr<Token> literal;
};

struct NonTerminal {
    std::vector<std::variant<Epsilon, Terminal, NonTerminal>> children;
};

class Production {
  public:
    using Symbol = std::variant<Production *, Token::Id, char, std::string>;
    using Node = std::variant<Epsilon, Terminal, NonTerminal>;

    std::string name;
    static bool debug;

    Production() = default;
    Production(std::string name) : name(std::move(name)) {}

    void add(std::initializer_list<Symbol> symbols);
    auto nullable() const -> bool;
    auto first() const -> std::unordered_set<Symbol>;
    auto produce(std::vector<std::unique_ptr<Token>> &tokens) -> Node;
    auto produce(ParseContext &ctx, bool isGoal = false) -> Node;

    friend auto operator<<(std::ostream &os,
                           const Production &p) -> std::ostream &;
    friend auto operator<<(std::ostream &os,
                           const std::vector<Symbol> &rule) -> std::ostream &;
    friend auto operator<<(std::ostream &os, const Symbol &s) -> std::ostream &;
    friend auto operator<<(std::ostream &os, const Node &n) -> std::ostream &;

  private:
    std::vector<std::vector<Symbol>> rules;
};

} // namespace parser

#include "parser/Production.tpp" // IWYU pragma: keep
