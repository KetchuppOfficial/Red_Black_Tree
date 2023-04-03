#include <gtest/gtest.h>
#include "nodes.hpp"

/*
 *   |               |
 *   x               y
 *  / \             / \
 * a   y    -->    x   c
 *    / \         / \
 *   b   c       a   b
 */
TEST (Details, Left_Tree_Rotation)
{
    using color_type = typename yLab::ARB_Node<int>::RB_Color;

    yLab::ARB_Node<int> root{1, color_type::red};
    yLab::ARB_Node<int> x{2, color_type::red};
    yLab::ARB_Node<int> y{3, color_type::red};
    yLab::ARB_Node<int> a{4, color_type::red};
    yLab::ARB_Node<int> b{5, color_type::red};
    yLab::ARB_Node<int> c{6, color_type::red};

    x.parent_ = &root;
    x.left_   = &a;
    x.right_  = &y;
    y.parent_ = &x;
    y.left_   = &b;
    y.right_  = &c;
    a.parent_ = &x;
    b.parent_ = &y;
    c.parent_ = &y;

    yLab::detail::left_rotate (&x);

    EXPECT_EQ (y.parent_, &root);
    EXPECT_EQ (y.left_,   &x);
    EXPECT_EQ (y.right_,  &c);
    EXPECT_EQ (x.parent_, &y);
    EXPECT_EQ (x.left_,   &a);
    EXPECT_EQ (x.right_,  &b);
    EXPECT_EQ (a.parent_, &x);
    EXPECT_EQ (a.left_,   nullptr);
    EXPECT_EQ (a.right_,  nullptr);
    EXPECT_EQ (b.parent_, &x);
    EXPECT_EQ (b.left_,   nullptr);
    EXPECT_EQ (b.right_,  nullptr);
    EXPECT_EQ (c.parent_, &y);
    EXPECT_EQ (c.left_,   nullptr);
    EXPECT_EQ (c.right_,  nullptr);
}

/*
 *   |               |
 *   y               x
 *  / \             / \
 * a   x    <--    y   c
 *    / \         / \
 *   b   c       a   b
 */
TEST (Details, Right_Tree_Rotation)
{
    using color_type = typename yLab::ARB_Node<int>::RB_Color;
    
    yLab::ARB_Node<int> root{1, color_type::black};
    yLab::ARB_Node<int> x{2, color_type::red};
    yLab::ARB_Node<int> y{3, color_type::red};
    yLab::ARB_Node<int> a{4, color_type::red};
    yLab::ARB_Node<int> b{5, color_type::red};
    yLab::ARB_Node<int> c{6, color_type::red};

    y.parent_ = &root;
    y.left_   = &x;
    y.right_  = &c;
    x.parent_ = &y;
    x.left_   = &a;
    x.right_  = &b;
    a.parent_ = &x;
    b.parent_ = &x;
    c.parent_ = &y;

    yLab::detail::right_rotate (&y);

    EXPECT_EQ (x.parent_, &root);
    EXPECT_EQ (x.left_,   &a);
    EXPECT_EQ (x.right_,  &y);
    EXPECT_EQ (y.parent_, &x);
    EXPECT_EQ (y.left_,   &b);
    EXPECT_EQ (y.right_,  &c);
    EXPECT_EQ (a.parent_, &x);
    EXPECT_EQ (a.left_,   nullptr);
    EXPECT_EQ (a.right_,  nullptr);
    EXPECT_EQ (b.parent_, &y);
    EXPECT_EQ (b.left_,   nullptr);
    EXPECT_EQ (b.right_,  nullptr);
    EXPECT_EQ (c.parent_, &y);
    EXPECT_EQ (c.left_,   nullptr);
    EXPECT_EQ (c.right_,  nullptr);
}
