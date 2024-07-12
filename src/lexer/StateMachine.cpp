#include "StateMachine.hpp"
#include "Node.hpp"
#include "State.hpp"
#include <cassert>
#include <memory>

StateMachine::StateMachine(std::unique_ptr<Node> n)
{
    assert(State::Enter == 0);
    assert(State::Accept == 1);
    assert(State::Reject == 2);

    // Enter
    states.push_back(std::make_unique<PredState>([](char) { return true; }));
    states[State::Enter]->id = State::Enter;
    // Accept
    states.push_back(std::make_unique<PredState>([](char) { return true; }));
    states[State::Accept]->id = State::Accept;
    // Reject
    states.push_back(std::make_unique<PredState>([](char) { return true; }));
    states[State::Reject]->id = State::Reject;

    for (auto state : n->states) {
        state->id = states.size();
        states.push_back(state);
    }
    for (auto e : n->entry) {
        states[State::Enter]->addEdge(e);
    }
    for (auto x : n->exit) {
        x->addEdge(states[State::Accept]);
    }
    for (auto state : states) {
        state->addEdge(states[State::Reject]);
    }
}

int StateMachine::transition(int state, char c) const
{
    if (state == State::Accept || state == State::Reject) {
        return State::Reject;
    }

    std::shared_ptr<State> currState = states.at(state);
    std::shared_ptr<State> nextState = currState->transition(c);
    if (nextState == nullptr) {
        return State::Reject;
    }
    return nextState->id;
}
