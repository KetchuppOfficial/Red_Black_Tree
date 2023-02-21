#include <gtest/gtest.h>
#include "details.hpp"

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
    yLab::RB_Node<int> root{1, yLab::RB_Color::black};
    yLab::RB_Node<int> x{2, yLab::RB_Color::red};
    yLab::RB_Node<int> y{3, yLab::RB_Color::red};
    yLab::RB_Node<int> a{4, yLab::RB_Color::red};
    yLab::RB_Node<int> b{5, yLab::RB_Color::red};
    yLab::RB_Node<int> c{6, yLab::RB_Color::red};

    x.parent_ = &root;
    x.left_   = &a;
    x.right_  = &y;
    y.parent_ = &x;
    y.left_   = &b;
    y.right_  = &c;
    a.parent_ = &x;
    b.parent_ = &y;
    c.parent_ = &y;

    yLab::details::left_rotate (&x);

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
 *   x               y
 *  / \             / \
 * a   y    <--    x   c
 *    / \         / \
 *   b   c       a   b
 */
TEST (Details, Right_Tree_Rotation)
{
    yLab::RB_Node<int> root{1, yLab::RB_Color::black};
    yLab::RB_Node<int> x{2, yLab::RB_Color::red};
    yLab::RB_Node<int> y{3, yLab::RB_Color::red};
    yLab::RB_Node<int> a{4, yLab::RB_Color::red};
    yLab::RB_Node<int> b{5, yLab::RB_Color::red};
    yLab::RB_Node<int> c{6, yLab::RB_Color::red};

    y.parent_ = &root;
    y.left_   = &x;
    y.right_  = &c;
    x.parent_ = &y;
    x.left_   = &a;
    x.right_  = &b;
    a.parent_ = &x;
    b.parent_ = &x;
    c.parent_ = &y;

    yLab::details::right_rotate (&y);

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
