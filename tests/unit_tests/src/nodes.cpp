#include <gtest/gtest.h>

#include <nodes.hpp>

TEST (Nodes, Requirements)
{
    static_assert (yLab::Binary_Tree_Node<yLab::ARB_Node<int>>);
}
