#include <gtest/gtest.h>

#include <nodes.hpp>

TEST (Nodes, Requirements)
{
    static_assert (yLab::Binary_Tree_Node<yLab::BT_Node<int>>);
    static_assert (yLab::Binary_Tree_Node<yLab::RB_Node<int>>);
    static_assert (yLab::Binary_Tree_Node<yLab::RB_Node<int>>);
}
