#include "lexer/StateMachine.hpp"
#include "lexer/Node.hpp"
#include "lexer/State.hpp"
#include <cassert>
#include <memory>

using lexer::StateMachine;

StateMachine::StateMachine(std::unique_ptr<Node> n)
{
    static_assert(State::Enter == 0);
    static_assert(State::Accept == 1);
    static_assert(State::Reject == 2);

    // Enter
    states.push_back(std::make_unique<PredState>([](char) { return true; }));
    states[State::Enter]->id = State::Enter;
    // Accept
    states.push_back(std::make_unique<PredState>([](char) { return true; }));
    states[State::Accept]->id = State::Accept;
    // Reject
    states.push_back(std::make_unique<PredState>([](char) { return true; }));
    states[State::Reject]->id = State::Reject;

    for (const auto &state : n->states) {
        state->id = states.size();
        states.push_back(state);
    }
    for (const auto &e : n->entry) {
        states[State::Enter]->addEdge(e);
    }
    for (const auto &x : n->exit) {
        x->addEdge(states[State::Accept]);
    }
    for (const auto &state : states) {
        state->setRejector(states[State::Reject]);
    }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto StateMachine::transition(int state, char c) const -> int
{
    if (state == State::Accept || state == State::Reject) {
        return State::Reject;
    }

    std::shared_ptr<State> currState = states.at(state);
    std::shared_ptr<State> nextState = currState->transition(c);
    if (nextState == nullptr) {
        return State::Reject;
    }
    return static_cast<int>(nextState->id);
}
