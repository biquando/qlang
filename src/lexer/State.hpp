#pragma once

#include <functional>
#include <memory>
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
    void setRejector(std::shared_ptr<State> rejector)
    {
        this->rejector = rejector;
    }

    friend std::ostream &operator<<(std::ostream &o, const State &n);
    virtual void print(std::ostream &o) const;

  protected:
    std::vector<std::shared_ptr<State>> successors;
    std::vector<std::shared_ptr<State>> epsilonSuccessors;
    std::shared_ptr<State> rejector;
};

struct CharState : public State {
    char literal;
    CharState(char literal) : literal(literal) {}
    virtual bool matches(char c) const override { return c == literal; }
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
