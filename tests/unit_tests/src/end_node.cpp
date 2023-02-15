#include <gtest/gtest.h>
#include "rb_tree.hpp"


TEST (End_Node, Move_Constructor)
{
    int x{};
    yLab::End_Node<decltype(x) *> end_node_1{&x};
    auto end_node_2 = std::move (end_node_1);

    EXPECT_EQ (end_node_1.left_, nullptr);
    EXPECT_EQ (end_node_2.left_, &x);
}

TEST (End_Node, Move_Assignment)
{
    int x{};
    yLab::End_Node<decltype(x) *> end_node_1{&x};

    int y{};
    yLab::End_Node<decltype(y) *> end_node_2{&y};

    end_node_2 = std::move (end_node_1);
    EXPECT_EQ (end_node_1.left_, &y);
    EXPECT_EQ (end_node_2.left_, &x);
}
