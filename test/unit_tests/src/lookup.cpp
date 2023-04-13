#include <gtest/gtest.h>
#include <iterator>

#include "arb_tree.hpp"

TEST (Lookup, Find)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5, 6};

    auto key = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++key)
        EXPECT_EQ (it, tree.find (key));

    yLab::ARB_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.find(0), empty_tree.end());
}

TEST (Lookup, Contains)
{
    yLab::ARB_Tree tree = {1, 2, 3, 4, 5, 6};

    EXPECT_FALSE (tree.contains (0));
    EXPECT_FALSE (tree.contains (7));
    EXPECT_TRUE (std::all_of (tree.begin(), tree.end(),
                              [&tree](auto &&key){ return tree.contains(key); }));

    yLab::ARB_Tree<int> empty_tree;
    EXPECT_FALSE (empty_tree.contains(0));
}

TEST (Lookup, Lower_Bound)
{
    yLab::ARB_Tree tree = {1, 3};

    EXPECT_EQ (tree.lower_bound (0), tree.find (1));
    EXPECT_EQ (tree.lower_bound (1), tree.find (1));
    EXPECT_EQ (tree.lower_bound (2), tree.find (3));
    EXPECT_EQ (tree.lower_bound (3), tree.find (3));
    EXPECT_EQ (tree.lower_bound (4), tree.end());

    yLab::ARB_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.lower_bound (0), empty_tree.end());
}

TEST (Lookup, Upper_Bound)
{
    yLab::ARB_Tree tree = {1, 3};

    EXPECT_EQ (tree.upper_bound (0), tree.find (1));
    EXPECT_EQ (tree.upper_bound (1), tree.find (3));
    EXPECT_EQ (tree.upper_bound (2), tree.find (3));
    EXPECT_EQ (tree.upper_bound (3), tree.end());
    EXPECT_EQ (tree.upper_bound (4), tree.end());

    yLab::ARB_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.upper_bound (0), empty_tree.end());
}

TEST (Lookup, Kth_Smallest)
{
    yLab::ARB_Tree tree = {0, 2, 4, 6, 8, 10, 12};

    for (auto k = 1; k != tree.size() + 1; ++k)
        EXPECT_EQ (tree.kth_smallest (k), std::next (tree.begin(), k - 1));

    EXPECT_EQ (tree.kth_smallest (0), tree.end());
    EXPECT_EQ (tree.kth_smallest (tree.size() + 1), tree.end());

    yLab::ARB_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.kth_smallest (1), empty_tree.end());
}

TEST (Lookup, N_Less_Than)
{
    yLab::ARB_Tree tree = {0, 2, 4, 6, 8, 10, 12};

    auto n = 0;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++n)
        EXPECT_EQ (tree.n_less_than (*it), n);

    EXPECT_EQ (tree.n_less_than (-1), 0);
    EXPECT_EQ (tree.n_less_than (5), 3);
    EXPECT_EQ (tree.n_less_than (14), tree.size());
}
