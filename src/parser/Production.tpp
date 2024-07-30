#pragma once

#include "IndentedStream.hpp"
#include "Production.hpp"
#include <iostream>
#include <unordered_set>
#include <variant>
#include <vector>

bool Production::debug = false;
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

void Production::add(std::initializer_list<Symbol> symbols)
{
    rules.emplace_back(symbols.begin(), symbols.end());
}

bool Production::nullable() const
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

static std::unordered_set<Production::Symbol>
ruleFirst(std::vector<Production::Symbol> rule)
{
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

std::unordered_set<Production::Symbol> Production::first() const
{
    std::unordered_set<Symbol> firstSet;
    for (const std::vector<Symbol> &rule : rules) {
        std::unordered_set<Symbol> ruleFirstSet = ruleFirst(rule);
        firstSet.insert(ruleFirstSet.begin(), ruleFirstSet.end());
    }
    return firstSet;
}

Production::Node Production::produce(ParseContext &ctx)
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
            return Epsilon();
        }

        // TODO: This should go in a helper function
        std::unordered_set<Symbol> ruleFirstSet = ruleFirst(rule);
        bool ruleMatches = false;
        for (const Symbol &symbol : ruleFirstSet) {
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

        if (ruleMatches) {
            DBG_OS(ios) << "Chosen rule: " << rule << "\n";
            for (const Symbol &symbol : rule) {
                if (std::holds_alternative<Production *>(symbol)) {
                    Production *prod = std::get<Production *>(symbol);
                    // TODO: Don't add if prod->produce() is Epsilon
                    children.push_back(prod->produce(ctx));
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
            return NonTerminal{std::move(children)};
        }
    }
    ctx.error();
    debug_depth--;
    return Epsilon();
}

std::ostream &operator<<(std::ostream &os, const Production &p)
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

std::ostream &operator<<(std::ostream &os,
                         const std::vector<Production::Symbol> &rule)
{
    os << "Rule[\n";
    IndentedStream ios(os);
    for (const Production::Symbol &s : rule) {
        ios << s << "\n";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Production::Symbol &s)
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
std::ostream &operator<<(std::ostream &os, const Production::Node &n)
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
