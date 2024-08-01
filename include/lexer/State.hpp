#pragma once

#include <functional>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

namespace lexer {

class State {
  public:
    enum {
        Enter = 0,
        Accept = 1,
        Reject = 2,
    };
    unsigned id;

    virtual ~State() = default;
    void addEdge(const std::shared_ptr<State> &other);
    auto transition(char c) const -> std::shared_ptr<State>;
    virtual auto matches(char c) const -> bool = 0;
    virtual auto isEpsilon() const -> bool { return false; }
    void setRejector(std::shared_ptr<State> rejector)
    {
        this->rejector = std::move(rejector);
    }

    friend auto operator<<(std::ostream &o, const State &n) -> std::ostream &;
    virtual void print(std::ostream &o) const;

  protected:
    std::vector<std::shared_ptr<State>> successors;
    std::vector<std::shared_ptr<State>> epsilonSuccessors;
    std::shared_ptr<State> rejector;
};

struct CharState : public State {
    char literal;
    CharState(char literal) : literal(literal) {}
    auto matches(char c) const -> bool override { return c == literal; }
};

struct PredState : public State {
    std::function<bool(char)> literal;
    PredState(std::function<bool(char)> literal) : literal(std::move(literal))
    {
    }
    auto matches(char c) const -> bool override { return literal(c); }
};

struct EpsilonState : public State {
    EpsilonState() = default;
    auto isEpsilon() const -> bool override { return true; }
    auto matches(char /*c*/) const -> bool override { return true; }
};

} // namespace lexer
