#include <gtest/gtest.h>
#include <rb_tree.hpp>

TEST (Iterators, Check_Iterator_Concept)
{
    static_assert (std::bidirectional_iterator<yLab::RB_Tree<int>::iterator>);
    static_assert (std::bidirectional_iterator<yLab::RB_Tree<int>::const_iterator>);
}
