#include <gtest/gtest.h>

#include "rb_tree.hpp"

TEST (Erase, By_Iterator)
{
    yLab::RB_Tree tree = {15, 2, 1, 8, 3, 5, 7, 9, 4, 11};
    std::set<int> model{tree.begin(), tree.end()};

    auto it = tree.find (15);
    tree.erase (it);
    model.erase (15);

    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));
}

TEST (Erase, By_Key)
{
    yLab::RB_Tree tree = {1, 5, -20, 256, 11, 42, 89, 0};
    std::set<int> model{tree.begin(), tree.end()};

    auto is_erased = tree.erase (11);
    EXPECT_EQ (is_erased, 1);

    model.erase (11);
    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));

    is_erased = tree.erase (-1);
    EXPECT_EQ (is_erased, 0);
}
