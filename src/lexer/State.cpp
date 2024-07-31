#include "lexer/State.hpp"
#include <cassert>
#include <iostream>
#include <memory>

using lexer::EpsilonState;
using lexer::State;

std::ostream &operator<<(std::ostream &o, const State &n)
{
    n.print(o);
    return o;
}

void State::print(std::ostream &o) const
{
    o << "State(\n  id:     " << id << "\n";
    o << "  succ:  ";
    for (auto succ : successors) {
        o << " " << succ->id;
    }
    o << "\n";
    o << "  epSucc:";
    for (auto succ : epsilonSuccessors) {
        o << " " << succ->id;
    }
    o << "\n)";
}

void State::addEdge(std::shared_ptr<State> other)
{
    if (other->isEpsilon()) {
        epsilonSuccessors.push_back(other);
    }
    else {
        successors.push_back(other);
    }
}

std::shared_ptr<State> State::transition(char c) const
{
    std::shared_ptr<State> next = nullptr;
    for (std::shared_ptr<State> other : successors) {
        if (other->matches(c)) {
            next = other;
            break;
        }
    }
    if (next != nullptr) {
        return next;
    }

    for (std::shared_ptr<State> other : epsilonSuccessors) {
        assert(other->isEpsilon());
        if (other->matches(c)) {
            next = other;
            break;
        }
    }
    if (next != nullptr) {
        return next->transition(c);
    }

    return rejector;
}

// Other match functions are in header file
bool EpsilonState::matches(char c) const
{
    return true;
    for (std::shared_ptr<State> other : successors) {
        if (other->matches(c)) {
            return true;
        }
    }
    return false;
}
