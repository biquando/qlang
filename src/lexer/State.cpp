#include "State.hpp"
#include <memory>

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
        if (other->matches(c)) {
            next = other;
            break;
        }
    }
    if (next != nullptr) {
        return next->transition(c);
    }

    return nullptr;
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
