#include <gtest/gtest.h>
#include <rb_tree.hpp>

TEST (Iterators, Check_Iterator_Concept)
{
    static_assert (std::bidirectional_iterator<yLab::RB_Iterator<int>>);
    static_assert (std::bidirectional_iterator<yLab::const_RB_Iterator<int>>);
}
