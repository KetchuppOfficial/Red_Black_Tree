#include <gtest/gtest.h>

#include "rb_tree.hpp"

TEST (Modifiers, Clear)
{
    yLab::RB_Tree tree = {1, 2, 3, 4, 5, 6};
    yLab::RB_Tree<int> empty_tree;
    tree.clear();

    EXPECT_EQ (tree, empty_tree);
}

TEST (Modifiers, Insert_By_Key)
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

TEST (Modifiers, Insert_Range)
{
    std::set model = {1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};    

    yLab::RB_Tree<int> tree;
    tree.insert (model.begin(), model.end());

    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));
}

TEST (Modifiers, Insert_By_Initializer_List)
{
    std::set model = {1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};    

    yLab::RB_Tree<int> tree;
    tree.insert ({1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35});

    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));
}

TEST (Modifiers, Erase_By_Iterator)
{
    yLab::RB_Tree tree = {15, 2, 1, 8, 3, 5, 7, 9, 4, 11};
    std::set<int> model{tree.begin(), tree.end()};

    auto it = tree.find (15);
    tree.erase (it);
    model.erase (15);

    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), model.begin()));
}

TEST (Modifiers, Erase_By_Key)
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