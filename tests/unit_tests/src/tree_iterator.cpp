#include <gtest/gtest.h>
#include <rb_tree.hpp>
#include <utility>

TEST (Iterators, Check_Iterator_Concept)
{
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<int, yLab::ARB_Node<int>>>);
}

TEST (Iterators, Preincrement)
{
    yLab::RB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_3 = ++it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterators, Postincrement)
{
    yLab::RB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_1_copy = it_1;
    auto it_3 = it_1++;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterators, Predecrement)
{
    yLab::RB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_3 = --it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterators, Postdecrement)
{
    yLab::RB_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_1_copy = it_1;
    auto it_3 = it_1--;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterators, Dereference)
{
    yLab::RB_Tree tree = {1, 2, 3, 4, 5};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
        EXPECT_EQ (*it, elem);
}

TEST (Iterators, Arrow)
{
    yLab::RB_Tree<std::pair<int, int>> tree = {{1, -1}, {2, -2}, {3, -3}};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
    {
        EXPECT_EQ (it->first, elem);
        EXPECT_EQ (it->second, -elem);
    }  
}
