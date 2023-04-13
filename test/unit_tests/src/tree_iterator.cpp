#include <gtest/gtest.h>
#include <utility>

#include "arb_tree.hpp"

TEST (Iterators, Check_Iterator_Concept)
{
    using node_type = yLab::ARB_Node<int>;
    using end_node_type = yLab::End_Node<node_type>;
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<int, node_type, end_node_type>>);
}

TEST (Iterators, Preincrement)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_3 = ++it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterators, Postincrement)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_1_copy = it_1;
    auto it_3 = it_1++;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterators, Predecrement)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_3 = --it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterators, Postdecrement)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_1_copy = it_1;
    auto it_3 = it_1--;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterators, Dereference)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
        EXPECT_EQ (*it, elem);
}

TEST (Iterators, Arrow)
{
    yLab::ARB_Tree<std::pair<int, int>> tree = {{1, -1}, {2, -2}, {3, -3}};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
    {
        EXPECT_EQ (it->first, elem);
        EXPECT_EQ (it->second, -elem);
    }  
}
