#include <gtest/gtest.h>

#include <nodes.hpp>

TEST (Nodes, Is_Left_Child)
{
    using node_type = yLab::ARB_Node<int>;
    using color_type = typename node_type::color_type;
    
    node_type parent{1, color_type::red};
    node_type left{1, color_type::black};
    node_type right{1, color_type::black};

    parent.left_ = &left;
    parent.right_ = &right;
    left.parent_ = &parent;
    right.parent_ = &parent;

    EXPECT_TRUE (yLab::detail::is_left_child (&left));
    EXPECT_FALSE (yLab::detail::is_left_child (&right));

    // Also works for end_node
    yLab::End_Node<node_type> end_node;
    end_node.left_ = &left;
    left.parent_ = &end_node;

    EXPECT_TRUE (yLab::detail::is_left_child (&left));
}

/*
 *    +-- 4 --+
 *    |       |
 *    2       6
 *   / \     / \.
 *  1   3   5   7
 */
TEST (Nodes, Basic_Queries)
{
    using node_type = yLab::ARB_Node<int>;
    using color_type = node_type::color_type;

    yLab::End_Node<node_type> end_node;
    node_type top{4, color_type::black};
    auto end_node_ptr = &end_node;

    end_node.left_ = &top;
    top.parent_ = end_node_ptr;

    node_type l_node{2, color_type::red};
    node_type r_node{6, color_type::red};

    top.left_ = &l_node;
    top.right_ = &r_node;
    l_node.parent_ = &top;
    r_node.parent_ = &top;

    node_type ll_node{1, color_type::black};
    node_type lr_node{3, color_type::black};

    l_node.left_ = &ll_node;
    l_node.right_ = &lr_node;
    ll_node.parent_ = &l_node;
    lr_node.parent_ = &l_node;

    node_type rl_node{5, color_type::black};
    node_type rr_node{7, color_type::black};

    r_node.left_ = &rl_node;
    r_node.right_ = &rr_node;
    rl_node.parent_ = &r_node;
    rr_node.parent_ = &r_node;

    // Minimum and maximum

    EXPECT_EQ (yLab::detail::minimum (&top), &ll_node);
    EXPECT_EQ (yLab::detail::maximum (&top), &rr_node);

    EXPECT_EQ (yLab::detail::minimum (&l_node), &ll_node);
    EXPECT_EQ (yLab::detail::maximum (&l_node), &lr_node);

    EXPECT_EQ (yLab::detail::minimum (&ll_node), &ll_node);
    EXPECT_EQ (yLab::detail::maximum (&ll_node), &ll_node);

    EXPECT_EQ (yLab::detail::minimum (&lr_node), &lr_node);
    EXPECT_EQ (yLab::detail::maximum (&lr_node), &lr_node);

    EXPECT_EQ (yLab::detail::minimum (&r_node), &rl_node);
    EXPECT_EQ (yLab::detail::maximum (&r_node), &rr_node);

    EXPECT_EQ (yLab::detail::minimum (&rl_node), &rl_node);
    EXPECT_EQ (yLab::detail::maximum (&rl_node), &rl_node);

    EXPECT_EQ (yLab::detail::minimum (&rr_node), &rr_node);
    EXPECT_EQ (yLab::detail::maximum (&rr_node), &rr_node);

    // Successor and predecessor

    EXPECT_EQ (yLab::detail::successor (&top), &rl_node);
    EXPECT_EQ (yLab::detail::predecessor (&top), &lr_node);

    EXPECT_EQ (yLab::detail::successor (&l_node), &lr_node);
    EXPECT_EQ (yLab::detail::predecessor (&l_node), &ll_node);

    EXPECT_EQ (yLab::detail::successor (&ll_node), &l_node);
    // predecessor for node with smallest key is undefined

    EXPECT_EQ (yLab::detail::successor (&lr_node), &top);
    EXPECT_EQ (yLab::detail::predecessor (&lr_node), &l_node);

    EXPECT_EQ (yLab::detail::successor (&r_node), &rr_node);
    EXPECT_EQ (yLab::detail::predecessor (&r_node), &rl_node);

    EXPECT_EQ (yLab::detail::successor (&rl_node), &r_node);
    EXPECT_EQ (yLab::detail::predecessor (&rl_node), &top);

    EXPECT_EQ (yLab::detail::successor (&rr_node), end_node_ptr);
    EXPECT_EQ (yLab::detail::predecessor (&rr_node), &r_node);
}

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
    using node_type = yLab::ARB_Node<int>;
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
    using node_type = yLab::ARB_Node<int>;
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
