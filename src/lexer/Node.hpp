#pragma once

#include "State.hpp"
#include <memory>

struct Node {
    std::vector<std::shared_ptr<State>> states;
    std::vector<std::shared_ptr<State>> entry;
    std::vector<std::shared_ptr<State>> exit;
    virtual ~Node() = default;
    virtual void connect() = 0;

    friend std::ostream &operator<<(std::ostream &o, const Node &n);
    virtual void print(std::ostream &o) const;
};

struct LiteralNode : Node {
    LiteralNode(std::shared_ptr<State> s);
    virtual void connect() override;
    virtual void print(std::ostream &o) const override;
};

struct ConcatNode : Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    ConcatNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    virtual void connect() override;
    virtual void print(std::ostream &o) const override;
};

struct AlternateNode : Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    AlternateNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    virtual void connect() override;
    virtual void print(std::ostream &o) const override;
};

struct PlusNode : Node {
    std::unique_ptr<Node> opr;
    PlusNode(std::unique_ptr<Node> opr);
    virtual void connect() override;
    virtual void print(std::ostream &o) const override;
};

struct StarNode : Node {
    std::unique_ptr<Node> opr;
    StarNode(std::unique_ptr<Node> opr);
    virtual void connect() override;
    virtual void print(std::ostream &o) const override;
};
