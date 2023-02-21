#ifndef INCLUDE_DETAILS_HPP
#define INCLUDE_DETAILS_HPP

#include <cassert>
#include <utility>

#include "nodes.hpp"

namespace yLab
{

namespace details
{

template<typename Key_T>
bool is_left_child (const RB_Node<Key_T> *node) noexcept
{
    assert (node && node->parent_);
    
    return node == node->parent_->left_;
}

template<typename Key_T>
const RB_Node<Key_T> *minimum (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    while (node->left_)
        node = node->left_;

    return node;
}

template<typename Key_T>
RB_Node<Key_T> *minimum (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(minimum (static_cast<const RB_Node<Key_T> *>(node)));
}

template<typename Key_T>
const RB_Node<Key_T> *maximum (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    while (node->right_)
        node = node->right_;

    return node;
}

template<typename Key_T>
RB_Node<Key_T> *maximum (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(maximum (static_cast<const RB_Node<Key_T> *>(node)));
}

template<typename Key_T>
const RB_Node<Key_T> *successor (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    if (node->right_)
        return minimum (node->right_);

    // If there is no right subtree of root_, than root->parent_->left == root_.
    // It implies that while loop body will not execute
    while (!is_left_child (node))
        node = node->parent_;

    return node->parent_;
}

template<typename Key_T>
RB_Node<Key_T> *successor (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(successor (static_cast<const RB_Node<Key_T> *>(node)));
}

template<typename Key_T>
const RB_Node<Key_T> *predecessor (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    if (node->left_)
        return maximum (node->left_);

    auto parent = node->parent;
    while (is_left_child (node))
        node = node->parent_;

    return node->parent_;
}

template<typename Key_T>
RB_Node<Key_T> *predecessor (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(prececessor (static_cast<const RB_Node<Key_T> *>(node)));
}

template <typename Key_T>
const RB_Node<Key_T> *find (const RB_Node<Key_T> *node, const Key_T &key)
{
    while (node && key != node->key())
        node = (key < node->key()) ? node->left_ : node->right_;

    return node;
}

template <typename Key_T>
RB_Node<Key_T> *find (RB_Node<Key_T> *node, const Key_T &key)
{
    return const_cast<RB_Node<Key_T> *>(find (static_cast<const RB_Node<Key_T> *>(node), key));
}

// Finds first element that is not less than key
template <typename Key_T>
const RB_Node<Key_T> *lower_bound (const RB_Node<Key_T> *node, const Key_T &key)
{
    const RB_Node<Key_T> *result = nullptr;
    while (node)
    {
        if (key <= node->key())
        {
            result = node;
            node = node->left_;
        }
        else
            node = node->right_;
    }

    return result;
}

template <typename Key_T>
RB_Node<Key_T> *lower_bound (RB_Node<Key_T> *node, const Key_T &key)
{    
    return const_cast<RB_Node<Key_T> *>(lower_bound (static_cast<const RB_Node<Key_T> *>(node), key));
}

// Finds first element that is greater than key
template <typename Key_T>
const RB_Node<Key_T> *upper_bound (const RB_Node<Key_T> *node, const Key_T &key)
{
    const RB_Node<Key_T> *result = nullptr;
    while (node)
    {
        if (key < node->key())
        {
            result = node;
            node = node->left_;
        }
        else
            node = node->right_;
    }

    return result;
}

template <typename Key_T>
RB_Node<Key_T> *upper_bound (RB_Node<Key_T> *node, const Key_T &key)
{
    return const_cast<RB_Node<Key_T> *>(upper_bound (static_cast<const RB_Node<Key_T> *>(node), key));
}

// (parent == nullptr) ==> (key == root().key())
// (node != nullptr) ==> (parent != nullptr)
template <typename Key_T>
auto find_v2 (RB_Node<Key_T> *node, const Key_T &key)
{
    using node_ptr = RB_Node<Key_T> *;
    using result = std::pair<node_ptr, node_ptr>;
    
    node_ptr parent = nullptr;

    while (node)
    {
        if (key == node->key())
            return result{node, parent};
        
        parent = node;
        node = (key < node->key()) ? node->left_ : node = node->right_;
    }

    return result{node, parent};
}

// Sometimes root_ can be affected. So it has to be changed if necessary
template<typename Key_T>
void left_rotate (RB_Node<Key_T> *x)
{
    assert (x && x->right_);
    
    auto y = x->right_;

    x->right_ = y->left_;
    if (y->left_)
        y->left_->parent_ = x;

    y->parent_ = x->parent_;
    if (is_left_child (x))
        x->parent_->left_ = y;
    else
        x->parent_->right_ = y;

    y->left_ = x;
    x->parent_ = y;
}

// Sometimes root_ can be affected. So it has to be changed if necessary
template <typename Key_T>
void right_rotate (RB_Node<Key_T> *x)
{
    assert (x && x->left_);

    auto y = x->left_;

    x->left_ = y->right_;
    if (y->right_)
        y->right_->parent_ = x;

    y->parent_ = x->parent_;
    if (x == x->parent_->left_)
        x->parent_->left_ = y;
    else
        x->parent_->right_ = y;

    y->right_ = x;
    x->parent_ = y;
}

template <typename Key_T>
auto fixup_subroutine_1 (RB_Node<Key_T> *new_node, RB_Node<Key_T> *uncle, const RB_Node<Key_T> *root)
{
    new_node = new_node->parent_;
    new_node->color_ = RB_Color::black;

    new_node = new_node->parent_;
    if (new_node != root)
        new_node->color_ = RB_Color::red;

    uncle->color_ = RB_Color::black;

    return new_node;
}

template <typename Key_T>
auto fixup_subroutine_2 (RB_Node<Key_T> *new_node)
{
    new_node = new_node->parent_;
    new_node->color_ = RB_Color::black;

    new_node = new_node->parent_;
    new_node->color_ = RB_Color::red;

    return new_node;
}

// RB_invatiant (end_node_->left_) == true
// But end_node_->left_ may be different than the value passed ad root
template <typename Key_T>
void rb_insert_fixup (const RB_Node<Key_T> *root, RB_Node<Key_T> *new_node)
{       
    assert (root && new_node);
    
    // Checks if "The root is black" property violated
    if (new_node == root)
    {
        new_node->color_ = RB_Color::black;
        return;
    }

    // (new_node != root_) ==> (root_->color_ == RB_Color::black)

    // Checks if "If a node is red, then both its children are black" property violated
    while (new_node != root && new_node->parent_->color_ == RB_Color::red)
    {
        // First condition is important only for iterations 2, 3, ... but not for 1
        // (new_node->parent_->color_ == RB_Color::red) ==> (new_node->parent_ != root_)
        
        if (is_left_child (new_node->parent_))
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            RB_Node<Key_T> *uncle = new_node->parent_->parent_->right_;

            if (uncle && uncle->color_ == RB_Color::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (!is_left_child (new_node->parent_))
                {
                    new_node = new_node->parent_;
                    left_rotate (new_node);
                }

                right_rotate (fixup_subroutine_2 (new_node));
                break;
            }
        }
        else
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            RB_Node<Key_T> *uncle = new_node->parent_->parent_->left_;

            if (uncle && uncle->color_ == RB_Color::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (is_left_child (new_node->parent_))
                {
                    new_node = new_node->parent_;
                    details::right_rotate (new_node);
                }

                left_rotate (fixup_subroutine_2 (new_node));
                break;
            }
        }
    }
}

} // namespace details

} // namespace yLab

#endif // INCLUDE_DETAILS_HPP
