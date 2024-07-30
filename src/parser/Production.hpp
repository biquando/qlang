#pragma once

#include "ParseContext.hpp"
#include "Token.hpp"
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

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
    typedef std::variant<Production *, Token::Id, char, std::string> Symbol;
    typedef std::variant<Epsilon, Terminal, NonTerminal> Node;

    std::string name;
    static bool debug;

    Production() = default;
    Production(std::string name) : name(name) {}

    void add(std::initializer_list<Symbol> symbols);
    bool nullable() const;
    std::unordered_set<Symbol> first() const;
    Node produce(ParseContext &ctx);

    friend std::ostream &operator<<(std::ostream &os, const Production &p);
    friend std::ostream &operator<<(std::ostream &os,
                                    const std::vector<Symbol> &rule);
    friend std::ostream &operator<<(std::ostream &os, const Symbol &s);
    friend std::ostream &operator<<(std::ostream &os, const Node &n);

  private:
    std::vector<std::vector<Symbol>> rules;
};

} // namespace parser

#include "Production.tpp" // IWYU pragma: keep
