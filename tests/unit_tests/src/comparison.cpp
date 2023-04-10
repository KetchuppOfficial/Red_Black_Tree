#include <gtest/gtest.h>

#include "rb_tree.hpp"

TEST (Comparison, Equality)
{
    yLab::RB_Tree tree_1 = {1, 2, 3, 4, 5, 6};
    yLab::RB_Tree tree_2 = {1, 2, 3, 4, 5};
    yLab::RB_Tree tree_3 = {1, 2, 3, 4, 5, 6, 7};

    yLab::RB_Tree tree_4 = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ (tree_1, tree_1);
    EXPECT_EQ (tree_1, tree_4);
    EXPECT_EQ (tree_4, tree_1);

    EXPECT_NE (tree_1, tree_2);
    EXPECT_NE (tree_2, tree_1);
    EXPECT_NE (tree_2, tree_3);
    EXPECT_NE (tree_3, tree_2);
    EXPECT_NE (tree_3, tree_1);
    EXPECT_NE (tree_1, tree_3);
}

TEST (Comaprison, Ordering)
{
    yLab::RB_Tree tree_1 = {1, 2, 3, 4, 5};
    yLab::RB_Tree tree_2 = {1, 2, 3, 4, 5, 6};
    yLab::RB_Tree tree_3 = {1, 2, 3, 4};
    yLab::RB_Tree tree_4 = {1, 2, 3, 4, 6};
    yLab::RB_Tree tree_5 = {1, 2, 3, 4, -1};

    EXPECT_LT (tree_1, tree_2);
    EXPECT_GE (tree_2, tree_1);

    EXPECT_GT (tree_1, tree_3);
    EXPECT_LE (tree_3, tree_1);

    EXPECT_LT (tree_1, tree_4);
    EXPECT_GE (tree_4, tree_1);

    EXPECT_GT (tree_1, tree_5);
    EXPECT_LE (tree_5, tree_1);
}
