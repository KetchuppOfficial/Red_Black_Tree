#include <gtest/gtest.h>
#include <rb_tree.hpp>

TEST (Iterators, Check_Iterator_Concept)
{
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<int, yLab::RB_Node<int>>>);
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<int, yLab::RB_Node<int>>>);
}
