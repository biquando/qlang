#pragma once

#include "lexer/Node.hpp"
#include "lexer/State.hpp"
#include <memory>
#include <vector>

namespace lexer {

struct StateMachine {
    StateMachine(std::unique_ptr<Node> n);
    int transition(int state, char c) const;
    std::vector<std::shared_ptr<State>> states;
};

} // namespace lexer
