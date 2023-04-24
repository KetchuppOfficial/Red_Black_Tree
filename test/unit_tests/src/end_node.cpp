#include <gtest/gtest.h>

#include "nodes.hpp"

TEST (End_Node, Move_Constructor)
{
    int x{};
    yLab::End_Node end_node_1{&x};
    auto end_node_2 = std::move (end_node_1);

    EXPECT_EQ (end_node_1.get_left(), nullptr);
    EXPECT_EQ (end_node_2.get_left(), &x);
}

TEST (End_Node, Move_Assignment)
{
    int x{};
    yLab::End_Node end_node_1{&x};

    int y{};
    yLab::End_Node end_node_2{&y};

    end_node_2 = std::move (end_node_1);
    EXPECT_EQ (end_node_1.get_left(), &y);
    EXPECT_EQ (end_node_2.get_left(), &x);
}
