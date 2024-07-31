#pragma once

#include "parser/IndentedStream.hpp"
#include "parser/Production.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_set>
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

bool parser::Production::nullable() const
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

static std::unordered_set<parser::Production::Symbol>
ruleFirst(std::vector<parser::Production::Symbol> rule)
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

std::unordered_set<parser::Production::Symbol> parser::Production::first() const
{
    std::unordered_set<Symbol> firstSet;
    for (const std::vector<Symbol> &rule : rules) {
        std::unordered_set<Symbol> ruleFirstSet = ruleFirst(rule);
        firstSet.insert(ruleFirstSet.begin(), ruleFirstSet.end());
    }
    return firstSet;
}

parser::Production::Node
parser::Production::produce(std::vector<std::unique_ptr<Token>> &tokens)
{
    ParseContext ctx(tokens);
    return produce(ctx, true);
}

bool shouldUseRule(const std::vector<parser::Production::Symbol> &rule,
                   const parser::ParseContext &ctx)
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

static void checkGoal(parser::ParseContext &ctx, bool isGoal)
{
    if (isGoal && ctx.token) {
        ctx.error("Expected end of input");
    }
}

parser::Production::Node parser::Production::produce(ParseContext &ctx,
                                                     bool isGoal)
{
    static int debug_depth = -1;
    debug_depth++;
    IndentedStream ios(std::cerr, debug_depth * 4);
    DBG_OS(ios) << "Producing " << *this << "\n";

    std::vector<Node> children;
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
            DBG_OS(ios) << "Eating symbol " << symbol << "\n";
            if (std::holds_alternative<Token::Id>(symbol)) {
                tok = ctx.eat(std::get<Token::Id>(symbol));
            }
            else if (std::holds_alternative<char>(symbol)) {
                tok = ctx.eat(std::get<char>(symbol));
            }
            else if (std::holds_alternative<std::string>(symbol)) {
                tok = ctx.eat(std::get<std::string>(symbol));
            }
            DBG_OS(ios) << "Ate: " << *tok << "\n";
            children.push_back(Terminal{std::move(tok)});
        }
        debug_depth--;
        assert(!children.empty());
        checkGoal(ctx, isGoal);
        if (children.size() == 1) {
            return std::move(children.at(0));
        }
        else {
            return NonTerminal{std::move(children)};
        }
    }

    std::stringstream ss;
    ss << "No rule in " << *this << " that matches token";
    ctx.error(ss.str());
    debug_depth--;
    checkGoal(ctx, isGoal);
    return Epsilon();
}

std::ostream &parser::operator<<(std::ostream &os, const parser::Production &p)
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

std::ostream &
parser::operator<<(std::ostream &os,
                   const std::vector<parser::Production::Symbol> &rule)
{
    os << "Rule[\n";
    IndentedStream ios(os);
    for (const Production::Symbol &s : rule) {
        ios << s << "\n";
    }
    os << "]";
    return os;
}

std::ostream &parser::operator<<(std::ostream &os,
                                 const parser::Production::Symbol &s)
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
std::ostream &parser::operator<<(std::ostream &os, const Production::Node &n)
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
