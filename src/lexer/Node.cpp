#include "lexer/Node.hpp"
#include "lexer/State.hpp"
#include <iostream>
#include <memory>

using lexer::AlternateNode;
using lexer::ConcatNode;
using lexer::LiteralNode;
using lexer::Node;
using lexer::OptionalNode;
using lexer::PlusNode;
using lexer::StarNode;

auto operator<<(std::ostream &o, const Node &n) -> std::ostream &
{
    n.print(o);
    return o;
}

void Node::print(std::ostream &o) const
{
    o << "Node[" << states.size() << "," << entry.size() << "," << exit.size()
      << "]";
}

LiteralNode::LiteralNode(const std::shared_ptr<State> &s)
{
    states.push_back(s);
    entry.push_back(s);
    exit.push_back(s);
    connect();
}

void LiteralNode::connect() {}

void LiteralNode::print(std::ostream &o) const
{
    o << "LiteralNode[" << states.size() << "," << entry.size() << ","
      << exit.size() << "]";
}

ConcatNode::ConcatNode(std::unique_ptr<Node> _left,
                       std::unique_ptr<Node> _right)
    : left(std::move(_left)), right(std::move(_right))
{
    for (const auto &ls : left->states) {
        states.push_back(ls);
    }
    for (const auto &rs : right->states) {
        states.push_back(rs);
    }
    for (const auto &le : left->entry) {
        entry.push_back(le);
    }
    for (const auto &rx : right->exit) {
        exit.push_back(rx);
    }

    connect();
}

void ConcatNode::connect()
{
    for (const auto &lx : left->exit) {
        for (const auto &re : right->entry) {
            lx->addEdge(re);
        }
    }
}

void ConcatNode::print(std::ostream &o) const
{
    o << "ConcatNode[" << states.size() << "," << entry.size() << ","
      << exit.size() << "]";
}

AlternateNode::AlternateNode(std::unique_ptr<Node> _left,
                             std::unique_ptr<Node> _right)
    : left(std::move(_left)), right(std::move(_right))
{
    for (const auto &ls : left->states) {
        states.push_back(ls);
    }
    for (const auto &rs : right->states) {
        states.push_back(rs);
    }
    for (const auto &le : left->entry) {
        entry.push_back(le);
    }
    for (const auto &re : right->entry) {
        entry.push_back(re);
    }
    for (const auto &lx : left->exit) {
        exit.push_back(lx);
    }
    for (const auto &rx : right->exit) {
        exit.push_back(rx);
    }

    connect();
}

void AlternateNode::connect() {}

void AlternateNode::print(std::ostream &o) const
{
    o << "AlternateNode[" << states.size() << "," << entry.size() << ","
      << exit.size() << "]";
}

PlusNode::PlusNode(std::unique_ptr<Node> _opr) : opr(std::move(_opr))
{
    states = opr->states;
    entry = opr->entry;
    exit = opr->exit;
    connect();
}

void PlusNode::connect()
{
    for (const auto &e : opr->entry) {
        for (const auto &x : opr->exit) {
            x->addEdge(e);
        }
    }
}

void PlusNode::print(std::ostream &o) const
{
    o << "PlusNode[" << states.size() << "," << entry.size() << ","
      << exit.size() << "]";
}

StarNode::StarNode(std::unique_ptr<Node> _opr) : opr(std::move(_opr))
{
    states = opr->states;
    entry = opr->entry;
    exit = opr->exit;

    std::shared_ptr<EpsilonState> epsilon = std::make_shared<EpsilonState>();
    states.push_back(epsilon);
    entry.push_back(epsilon);
    exit.push_back(epsilon);

    connect();
}

void StarNode::connect()
{
    for (const auto &e : opr->entry) {
        for (const auto &x : opr->exit) {
            x->addEdge(e);
        }
    }
}

void StarNode::print(std::ostream &o) const
{
    o << "StarNode[" << states.size() << "," << entry.size() << ","
      << exit.size() << "]";
}

OptionalNode::OptionalNode(std::unique_ptr<Node> _opr) : opr(std::move(_opr))
{
    states = opr->states;
    entry = opr->entry;
    exit = opr->exit;

    std::shared_ptr<EpsilonState> epsilon = std::make_shared<EpsilonState>();
    states.push_back(epsilon);
    entry.push_back(epsilon);
    exit.push_back(epsilon);

    connect();
}

void OptionalNode::connect() {}

void OptionalNode::print(std::ostream &o) const
{
    o << "OptionalNode[" << states.size() << "," << entry.size() << ","
      << exit.size() << "]";
}
