#include "lexer/State.hpp"
#include <cassert>
#include <iostream>
#include <memory>

using lexer::State;

auto operator<<(std::ostream &o, const State &n) -> std::ostream &
{
    n.print(o);
    return o;
}

void State::print(std::ostream &o) const
{
    o << "State(\n  id:     " << id << "\n";
    o << "  succ:  ";
    for (const auto &succ : successors) {
        o << " " << succ->id;
    }
    o << "\n";
    o << "  epSucc:";
    for (const auto &succ : epsilonSuccessors) {
        o << " " << succ->id;
    }
    o << "\n)";
}

void State::addEdge(const std::shared_ptr<State> &other)
{
    if (other->isEpsilon()) {
        epsilonSuccessors.push_back(other);
    }
    else {
        successors.push_back(other);
    }
}

auto State::transition(char c) const -> std::shared_ptr<State>
{
    std::shared_ptr<State> next = nullptr;
    for (const std::shared_ptr<State> &other : successors) {
        if (other->matches(c)) {
            next = other;
            break;
        }
    }
    if (next != nullptr) {
        return next;
    }

    for (const std::shared_ptr<State> &other : epsilonSuccessors) {
        assert(other->isEpsilon());
        next = other;
        break;
    }
    if (next != nullptr) {
        return next->transition(c);
    }

    return rejector;
}
