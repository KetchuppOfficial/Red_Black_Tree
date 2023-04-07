#include <gtest/gtest.h>
#include <set>
#include <algorithm>

#include "rb_tree.hpp"

TEST (Insert, By_Key)
{
    yLab::RB_Tree<int> tree;

    auto [it, flag] = tree.insert (1);

    EXPECT_TRUE (flag);
    EXPECT_EQ (tree.size(), 1);
    EXPECT_EQ (*it, 1);
    EXPECT_EQ (it, tree.begin());
    EXPECT_EQ (++it, tree.end());

    auto [it2, flag2] = tree.insert (1);

    EXPECT_FALSE (flag2);
    EXPECT_EQ (tree.size(), 1);
    EXPECT_EQ (*it2, 1);
    EXPECT_EQ (it2, tree.begin());
    EXPECT_EQ (++it2, tree.end());
}

TEST (Insert, Range)
{
    std::set model = {1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};    

    yLab::RB_Tree<int> tree;
    tree.insert (model.begin(), model.end());

    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));
}

TEST (Insert, Initializer_List)
{
    std::set model = {1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};    

    yLab::RB_Tree<int> tree;
    tree.insert ({1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35});

    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));
}
