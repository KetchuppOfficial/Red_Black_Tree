#include <gtest/gtest.h>

#include "nodes.hpp"

TEST (End_Node, Move_Constructor)
{
    int x{};
    yLab::End_Node end_node_1{&x};
    auto end_node_2 = std::move (end_node_1);

    EXPECT_EQ (end_node_1.get_left(), nullptr);
    EXPECT_EQ (end_node_2.get_left(), &x);
}

TEST (End_Node, Move_Assignment)
{
    int x{};
    yLab::End_Node end_node_1{&x};

    int y{};
    yLab::End_Node end_node_2{&y};

    end_node_2 = std::move (end_node_1);
    EXPECT_EQ (end_node_1.get_left(), &y);
    EXPECT_EQ (end_node_2.get_left(), &x);
}

TEST (ARB_Node, Move_Constructor)
{
    using key_type = int;
    using color_type = typename yLab::ARB_Node<key_type>::color_type;

    yLab::ARB_Node node_1{1, color_type::black};
    yLab::ARB_Node left{2, color_type::red};
    yLab::ARB_Node right{3, color_type::red};
    yLab::ARB_Node parent{4, color_type::black};
    node_1.set_left (&left);
    node_1.set_right (&right);
    node_1.set_parent (&parent);

    auto node_2 = std::move (node_1);

    EXPECT_EQ (node_1.get_left(), nullptr);
    EXPECT_EQ (node_1.get_right(), nullptr);
    EXPECT_EQ (node_1.get_parent(), nullptr);
    EXPECT_EQ (node_1.color_, color_type::black);
    EXPECT_EQ (node_1.key(), 1);

    EXPECT_EQ (node_2.get_left(), &left);
    EXPECT_EQ (node_2.get_right(), &right);
    EXPECT_EQ (node_2.get_parent(), &parent);
    EXPECT_EQ (node_2.color_, color_type::black);
    EXPECT_EQ (node_2.key(), 1);
}

TEST (ARB_Node, Move_Assignment)
{
    using key_type = int;
    using color_type = typename yLab::ARB_Node<key_type>::color_type;

    yLab::ARB_Node node_1{1, color_type::black};
    yLab::ARB_Node left_1{2, color_type::red};
    yLab::ARB_Node right_1{3, color_type::red};
    yLab::ARB_Node parent_1{4, color_type::black};
    node_1.set_left (&left_1);
    node_1.set_right (&right_1);
    node_1.set_parent (&parent_1);

    yLab::ARB_Node node_2{5, color_type::black};
    yLab::ARB_Node left_2{6, color_type::red};
    yLab::ARB_Node right_2{7, color_type::red};
    yLab::ARB_Node parent_2{8, color_type::black};
    node_2.set_left (&left_2);
    node_2.set_right (&right_2);
    node_2.set_parent (&parent_2);

    node_2 = std::move (node_1);

    EXPECT_EQ (node_1.get_left(), &left_2);
    EXPECT_EQ (node_1.get_right(), &right_2);
    EXPECT_EQ (node_1.get_parent(), &parent_2);
    EXPECT_EQ (node_1.color_, color_type::black);
    EXPECT_EQ (node_1.key(), 5);

    EXPECT_EQ (node_2.get_left(), &left_1);
    EXPECT_EQ (node_2.get_right(), &right_1);
    EXPECT_EQ (node_2.get_parent(), &parent_1);
    EXPECT_EQ (node_2.color_, color_type::black);
    EXPECT_EQ (node_2.key(), 1);
}
