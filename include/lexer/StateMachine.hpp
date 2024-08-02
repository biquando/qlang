#pragma once

#include <memory>
#include <vector>

#include "lexer/Node.hpp"
#include "lexer/State.hpp"

namespace lexer {

struct StateMachine {
    StateMachine(std::unique_ptr<Node> n);
    auto transition(int state, char c) const -> int;
    std::vector<std::shared_ptr<State>> states;
};

} // namespace lexer
