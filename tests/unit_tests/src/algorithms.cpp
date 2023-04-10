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
TEST (Details, Left_Rotate_Plus)
{
    using node_type = typename yLab::ARB_Node<int>;
    using color_type = typename node_type::color_type;

    node_type root{1, color_type::red};
    node_type x{2, color_type::red};
    node_type y{3, color_type::red};
    node_type a{4, color_type::red};
    node_type b{5, color_type::red};
    node_type c{6, color_type::red};

    auto a_size = 10;
    auto b_size = 20;
    auto c_size = 30;

    x.parent_ = &root;
    x.left_   = &a;
    x.right_  = &y;
    y.parent_ = &x;
    y.left_   = &b;
    y.right_  = &c;
    a.parent_ = &x;
    b.parent_ = &y;
    c.parent_ = &y;
    a.subtree_size_ = a_size;
    b.subtree_size_ = b_size;
    c.subtree_size_ = c_size;
    y.subtree_size_ = b_size + c_size + 1;
    x.subtree_size_ = a_size + y.subtree_size_ + 1;

    yLab::detail::left_rotate_plus (&x);

    EXPECT_EQ (y.parent_, &root);
    EXPECT_EQ (y.left_,   &x);
    EXPECT_EQ (y.right_,  &c);
    EXPECT_EQ (x.parent_, &y);
    EXPECT_EQ (x.left_,   &a);
    EXPECT_EQ (x.right_,  &b);
    EXPECT_EQ (a.parent_, &x);
    EXPECT_EQ (b.parent_, &x);
    EXPECT_EQ (c.parent_, &y);
    EXPECT_EQ (x.subtree_size_, a_size + b_size + 1);
    EXPECT_EQ (y.subtree_size_, x.subtree_size_ + c_size + 1);
}

/*
 *   |               |
 *   y               x
 *  / \             / \
 * a   x    <--    y   c
 *    / \         / \
 *   b   c       a   b
 */
TEST (Details, Right_Rotate_Plus)
{
    using node_type = typename yLab::ARB_Node<int>;
    using color_type = typename node_type::color_type;
    
    node_type root{1, color_type::black};
    node_type x{2, color_type::red};
    node_type y{3, color_type::red};
    node_type a{4, color_type::red};
    node_type b{5, color_type::red};
    node_type c{6, color_type::red};

    auto a_size = 10;
    auto b_size = 20;
    auto c_size = 30;

    x.parent_ = &root;
    x.left_   = &y;
    x.right_  = &c;
    y.parent_ = &x;
    y.left_   = &a;
    y.right_  = &b;
    a.parent_ = &y;
    b.parent_ = &y;
    c.parent_ = &x;
    a.subtree_size_ = a_size;
    b.subtree_size_ = b_size;
    c.subtree_size_ = c_size;
    y.subtree_size_ = a_size + b_size + 1;
    x.subtree_size_ = y.subtree_size_ + c_size + 1;

    yLab::detail::right_rotate_plus (&x);

    EXPECT_EQ (y.parent_, &root);
    EXPECT_EQ (y.left_,   &a);
    EXPECT_EQ (y.right_,  &x);
    EXPECT_EQ (x.parent_, &y);
    EXPECT_EQ (x.left_,   &b);
    EXPECT_EQ (x.right_,  &c);
    EXPECT_EQ (a.parent_, &y);
    EXPECT_EQ (b.parent_, &x);
    EXPECT_EQ (c.parent_, &x);
    EXPECT_EQ (x.subtree_size_, b_size + c_size + 1);
    EXPECT_EQ (y.subtree_size_, x.subtree_size_ + a_size + 1);
}
