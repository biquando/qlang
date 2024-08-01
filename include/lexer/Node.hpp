#pragma once

#include "lexer/State.hpp"
#include <memory>
#include <ostream>
#include <vector>

namespace lexer {

struct Node {
    std::vector<std::shared_ptr<State>> states;
    std::vector<std::shared_ptr<State>> entry;
    std::vector<std::shared_ptr<State>> exit;
    virtual ~Node() = default;
    virtual void connect() = 0;

    friend auto operator<<(std::ostream &o, const Node &n) -> std::ostream &;
    virtual void print(std::ostream &o) const;
};

struct LiteralNode : Node {
    LiteralNode(const std::shared_ptr<State> &s);
    void connect() override;
    void print(std::ostream &o) const override;
};

struct ConcatNode : Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    ConcatNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    void connect() override;
    void print(std::ostream &o) const override;
};

struct AlternateNode : Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    AlternateNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    void connect() override;
    void print(std::ostream &o) const override;
};

struct PlusNode : Node {
    std::unique_ptr<Node> opr;
    PlusNode(std::unique_ptr<Node> opr);
    void connect() override;
    void print(std::ostream &o) const override;
};

struct StarNode : Node {
    std::unique_ptr<Node> opr;
    StarNode(std::unique_ptr<Node> opr);
    void connect() override;
    void print(std::ostream &o) const override;
};

struct OptionalNode : Node {
    std::unique_ptr<Node> opr;
    OptionalNode(std::unique_ptr<Node> opr);
    void connect() override;
    void print(std::ostream &o) const override;
};

} // namespace lexer
