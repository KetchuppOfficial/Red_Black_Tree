#include <gtest/gtest.h>

#include <nodes.hpp>

TEST (Nodes, Is_Left_Child)
{
    using node_type = yLab::ARB_Node<int>;
    using color_type = typename node_type::color_type;
    
    node_type parent{1, color_type::red};
    node_type left{1, color_type::black};
    node_type right{1, color_type::black};

    parent.set_left (&left);
    parent.set_right (&right);
    left.set_parent (&parent);
    right.set_parent (&parent);

    EXPECT_TRUE (yLab::detail::is_left_child (&left));
    EXPECT_FALSE (yLab::detail::is_left_child (&right));

    // Also works for end_node
    yLab::End_Node<node_type> end_node;
    end_node.set_left (&left);
    left.set_parent (&end_node);

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

    end_node.set_left (&top);
    top.set_parent (end_node_ptr);

    node_type l_node{2, color_type::red};
    node_type r_node{6, color_type::red};

    top.set_left (&l_node);
    top.set_right (&r_node);
    l_node.set_parent (&top);
    r_node.set_parent (&top);

    node_type ll_node{1, color_type::black};
    node_type lr_node{3, color_type::black};

    l_node.set_left (&ll_node);
    l_node.set_right (&lr_node);
    ll_node.set_parent (&l_node);
    lr_node.set_parent (&l_node);

    node_type rl_node{5, color_type::black};
    node_type rr_node{7, color_type::black};

    r_node.set_left (&rl_node);
    r_node.set_right (&rr_node);
    rl_node.set_parent (&r_node);
    rr_node.set_parent (&r_node);

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

    x.set_parent (&root);
    x.set_left (&a);
    x.set_right (&y);
    y.set_parent (&x);
    y.set_left (&b);
    y.set_right (&c);
    a.set_parent (&x);
    b.set_parent (&y);
    c.set_parent (&y);
    a.subtree_size_ = a_size;
    b.subtree_size_ = b_size;
    c.subtree_size_ = c_size;
    y.subtree_size_ = b_size + c_size + 1;
    x.subtree_size_ = a_size + y.subtree_size_ + 1;

    yLab::detail::left_rotate (&x);

    EXPECT_EQ (y.get_parent(), &root);
    EXPECT_EQ (y.get_left(),   &x);
    EXPECT_EQ (y.get_right(),  &c);
    EXPECT_EQ (x.get_parent(), &y);
    EXPECT_EQ (x.get_left(),   &a);
    EXPECT_EQ (x.get_right(),  &b);
    EXPECT_EQ (a.get_parent(), &x);
    EXPECT_EQ (b.get_parent(), &x);
    EXPECT_EQ (c.get_parent(), &y);
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

    x.set_parent (&root);
    x.set_left (&y);
    x.set_right (&c);
    y.set_parent (&x);
    y.set_left (&a);
    y.set_right (&b);
    a.set_parent (&y);
    b.set_parent (&y);
    c.set_parent (&x);
    a.subtree_size_ = a_size;
    b.subtree_size_ = b_size;
    c.subtree_size_ = c_size;
    y.subtree_size_ = a_size + b_size + 1;
    x.subtree_size_ = y.subtree_size_ + c_size + 1;

    yLab::detail::right_rotate (&x);

    EXPECT_EQ (y.get_parent(), &root);
    EXPECT_EQ (y.get_left(),   &a);
    EXPECT_EQ (y.get_right(),  &x);
    EXPECT_EQ (x.get_parent(), &y);
    EXPECT_EQ (x.get_left(),   &b);
    EXPECT_EQ (x.get_right(),  &c);
    EXPECT_EQ (a.get_parent(), &y);
    EXPECT_EQ (b.get_parent(), &x);
    EXPECT_EQ (c.get_parent(), &x);
    EXPECT_EQ (x.subtree_size_, b_size + c_size + 1);
    EXPECT_EQ (y.subtree_size_, x.subtree_size_ + a_size + 1);
}
