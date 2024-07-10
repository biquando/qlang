#pragma once

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

class State {
  public:
    enum {
        Enter = 0,
        Accept = 1,
        Reject = 2,
    };

    unsigned id;
    virtual ~State() = default;
    void addEdge(std::shared_ptr<State> other);
    std::shared_ptr<State> transition(char c) const;
    virtual bool matches(char c) const = 0;
    virtual bool isEpsilon() const { return false; }

  protected:
    std::vector<std::shared_ptr<State>> successors;
    std::vector<std::shared_ptr<State>> epsilonSuccessors;
};

struct CharState : public State {
    char literal;
    CharState(char literal) : literal(literal) {}
    virtual bool matches(char c) const override { return c == literal; }
};

struct CharsetState : public State {
    std::unordered_set<char> literal;
    CharsetState(std::unordered_set<char> literal) : literal(literal) {}
    virtual bool matches(char c) const override { return literal.count(c); }
};

struct PredState : public State {
    std::function<bool(char)> literal;
    PredState(std::function<bool(char)> literal) : literal(literal) {}
    virtual bool matches(char c) const override { return literal(c); }
};

struct EpsilonState : public State {
    EpsilonState() = default;
    virtual bool isEpsilon() const override { return true; }
    virtual bool matches(char c) const override;
};
