#pragma once

#include "lexer/Node.hpp"
#include "lexer/State.hpp"
#include <memory>
#include <vector>

namespace lexer {

struct StateMachine {
    StateMachine(std::unique_ptr<Node> n);
    auto transition(int state, char c) const -> int;
    std::vector<std::shared_ptr<State>> states;
};

} // namespace lexer
