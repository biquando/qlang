#pragma once

#include "parser/IndentedStream.hpp"
#include "parser/ParseContext.hpp"
#include "parser/Production.hpp"
#include "parser/Token.hpp"
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

bool parser::Production::debug = false;
#define DBG                                                                    \
    if (!Production::debug) {                                                  \
    }                                                                          \
    else                                                                       \
        std::cerr
#define DBG_OS(os)                                                             \
    if (!Production::debug) {                                                  \
    }                                                                          \
    else                                                                       \
        os

void parser::Production::add(std::initializer_list<Symbol> symbols)
{
    rules.emplace_back(symbols.begin(), symbols.end());
}

auto parser::Production::nullable() const -> bool
{
    for (const std::vector<Symbol> &rule : rules) {
        if (rule.empty()) {
            return true;
        }

        bool containsNonNullable = false;
        for (const Symbol &symbol : rule) {
            if (std::holds_alternative<Production *>(symbol)) {
                if (!std::get<Production *>(symbol)->nullable()) {
                    containsNonNullable = true;
                    break;
                }
            }
            else {
                containsNonNullable = true;
                break;
            }
        }
        if (!containsNonNullable) {
            return true;
        }
    }
    return false;
}

static auto ruleFirst(const std::vector<parser::Production::Symbol> &rule)
    -> std::unordered_set<parser::Production::Symbol>
{
    using namespace parser;
    std::unordered_set<Production::Symbol> firstSet;
    for (const Production::Symbol &symbol : rule) {
        if (std::holds_alternative<Production *>(symbol)) {
            const Production *prod = std::get<Production *>(symbol);
            for (const Production::Symbol &s : prod->first()) {
                firstSet.insert(s);
            }
            if (prod->nullable()) {
                continue;
            }
        }
        else {
            firstSet.insert(symbol);
        }
        break;
    }
    return firstSet;
}

auto parser::Production::first() const
    -> std::unordered_set<parser::Production::Symbol>
{
    std::unordered_set<Symbol> firstSet;
    for (const std::vector<Symbol> &rule : rules) {
        std::unordered_set<Symbol> ruleFirstSet = ruleFirst(rule);
        firstSet.insert(ruleFirstSet.begin(), ruleFirstSet.end());
    }
    return firstSet;
}

auto parser::Production::produce(std::vector<std::unique_ptr<Token>> &tokens)
    -> parser::Production::Node
{
    ParseContext ctx(tokens);
    return produce(ctx, true);
}

static auto shouldUseRule(const std::vector<parser::Production::Symbol> &rule,
                          const parser::ParseContext &ctx) -> bool
{
    using namespace parser;
    std::unordered_set<Production::Symbol> ruleFirstSet = ruleFirst(rule);
    bool ruleMatches = false;
    for (const Production::Symbol &symbol : ruleFirstSet) {
        if (ctx.token == nullptr) {
            continue;
        }

        if (std::holds_alternative<Token::Id>(symbol)) {
            if (std::get<Token::Id>(symbol) == ctx.token->id()) {
                ruleMatches = true;
                break;
            }
        }
        else if (std::holds_alternative<char>(symbol)) {
            if (ctx.token->text.size() == 1 &&
                std::get<char>(symbol) == ctx.token->text[0]) {
                ruleMatches = true;
                break;
            }
        }
        else if (std::holds_alternative<std::string>(symbol)) {
            if (std::get<std::string>(symbol) == ctx.token->text) {
                ruleMatches = true;
                break;
            }
        }
    }
    return ruleMatches;
}

static auto
eatRule(const std::vector<parser::Production::Symbol> &rule,
        parser::ParseContext &ctx) -> std::vector<parser::Production::Node>
{
    using namespace parser;
    using Node = parser::Production::Node;
    using Symbol = parser::Production::Symbol;

    std::vector<Node> children;
    for (const Symbol &symbol : rule) {
        if (std::holds_alternative<Production *>(symbol)) {
            Production *prod = std::get<Production *>(symbol);
            Node n = prod->produce(ctx);
            if (!std::holds_alternative<Epsilon>(n)) {
                children.push_back(std::move(n));
            }
            continue;
        }

        std::unique_ptr<Token> tok;
        if (std::holds_alternative<Token::Id>(symbol)) {
            tok = ctx.eat(std::get<Token::Id>(symbol));
        }
        else if (std::holds_alternative<char>(symbol)) {
            tok = ctx.eat(std::get<char>(symbol));
        }
        else if (std::holds_alternative<std::string>(symbol)) {
            tok = ctx.eat(std::get<std::string>(symbol));
        }
        children.emplace_back(Terminal{std::move(tok)});
    }
    return children;
}

static void checkGoal(parser::ParseContext &ctx, bool isGoal)
{
    if (isGoal && ctx.token) {
        ctx.error("Expected end of input");
    }
}

auto parser::Production::produce(ParseContext &ctx,
                                 bool isGoal) -> parser::Production::Node
{
    static int debug_depth = -1;
    debug_depth++;
    IndentedStream ios(std::cerr, debug_depth * 4);
    DBG_OS(ios) << "Producing " << *this << "\n";

    for (std::vector<Symbol> &rule : rules) {
        if (rule.empty()) {
            DBG_OS(ios) << "(epsilon)\n";
            debug_depth--;
            checkGoal(ctx, isGoal);
            return Epsilon();
        }

        if (!shouldUseRule(rule, ctx)) {
            continue;
        }

        DBG_OS(ios) << "Chosen rule: " << rule << "\n";
        std::vector<Node> children = eatRule(rule, ctx);

        debug_depth--;
        assert(!children.empty());
        checkGoal(ctx, isGoal);
        if (children.size() == 1) {
            return std::move(children.at(0));
        }
        return NonTerminal{std::move(children)};
    }

    std::stringstream ss;
    ss << "No rule in " << *this << " that matches token";
    ctx.error(ss.str());
    debug_depth--;
    checkGoal(ctx, isGoal);
    return Epsilon();
}

auto parser::operator<<(std::ostream &os,
                        const parser::Production &p) -> std::ostream &
{
    if (!p.name.empty()) {
        os << "Production(" << p.name << ")";
        return os;
    }

    os << "Production[\n";
    IndentedStream ios2(os, 2);
    for (const std::vector<Production::Symbol> &rule : p.rules) {
        ios2 << rule << "\n";
    }
    os << "]";
    return os;
}

auto parser::operator<<(std::ostream &os,
                        const std::vector<parser::Production::Symbol> &rule)
    -> std::ostream &
{
    os << "Rule[\n";
    IndentedStream ios(os);
    for (const Production::Symbol &s : rule) {
        ios << s << "\n";
    }
    os << "]";
    return os;
}

auto parser::operator<<(std::ostream &os,
                        const parser::Production::Symbol &s) -> std::ostream &
{
    os << "Symbol(";
    if (std::holds_alternative<Production *>(s)) {
        os << *std::get<Production *>(s);
    }
    else if (std::holds_alternative<Token::Id>(s)) {
        os << "Token::Id " << std::get<Token::Id>(s);
    }
    else if (std::holds_alternative<char>(s)) {
        os << '\'' << std::get<char>(s) << '\'';
    }
    else if (std::holds_alternative<std::string>(s)) {
        os << '\"' << std::get<std::string>(s) << '\"';
    }
    os << ")";
    return os;
}
auto parser::operator<<(std::ostream &os,
                        const Production::Node &n) -> std::ostream &
{
    if (std::holds_alternative<Epsilon>(n)) {
        os << "Epsilon()";
    }
    else if (std::holds_alternative<Terminal>(n)) {
        os << "Terminal(" << *std::get<Terminal>(n).literal << ")";
    }
    else if (std::holds_alternative<NonTerminal>(n)) {
        os << "NonTerminal[\n";
        IndentedStream ios(os);
        for (const Production::Node &node : std::get<NonTerminal>(n).children) {
            ios << node << "\n";
        }
        os << "]";
    }
    return os;
}

// vim:ft=cpp
