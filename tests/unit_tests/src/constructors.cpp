#include <gtest/gtest.h>
#include "rb_tree.hpp"

TEST (Constructors, Default_Constructor)
{
    const yLab::RB_Tree<int> tree;
    EXPECT_EQ (tree.size(), 0);
    EXPECT_TRUE (tree.empty());
    EXPECT_EQ (tree.begin(), tree.end());
    EXPECT_EQ (tree.lower_bound(1), tree.end());
    EXPECT_EQ (tree.upper_bound(1), tree.end());
}

TEST (Constructors, Copy_Constructor)
{
    yLab::RB_Tree<int> tree {1, 2, 3, 4, 5};

    auto tree2 {tree};
    for (auto it1 = tree.begin(), it2 = tree2.begin(); 
         it1 != tree.end() && it2 != tree2.end(); ++it1, ++it2)
    {
        EXPECT_EQ (*it1, *it2);
    }
}

#if 0
TEST (Constructors, Move_Constructor)
{
    yLab::RB_Tree<int> tree;
    tree.insert({1, 2, 3, 4, 5});

    auto tree2 = std::move (tree);
}
#endif
