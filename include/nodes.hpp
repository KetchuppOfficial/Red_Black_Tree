#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include <type_traits>
#include <utility>
#include <cassert>

namespace yLab
{

template<typename T>
concept Binary_Tree_Node = 
requires { typename T::key_type; } &&
requires (T node)
{
    node.left_;   requires std::same_as<std::remove_const_t<decltype(node.left_)>,
                                        std::remove_const_t<T>*>;
    node.right_;  requires std::same_as<std::remove_const_t<decltype(node.right_)>,
                                        std::remove_const_t<T>*>;
    node.parent_; requires std::same_as<std::remove_const_t<decltype(node.parent_)>,
                                        std::remove_const_t<T>*>;

    { node.key() } -> std::same_as<const typename T::key_type &>;
};

// Node representing the result of end()
template<typename Node_T>
class End_Node
{
    using node_ptr = Node_T *;

public:

    node_ptr left_ = nullptr;

    End_Node () = default;

    End_Node (node_ptr left) : left_{left} {}

    End_Node (const End_Node &rhs) = delete;
    End_Node &operator= (const End_Node &rhs) = delete;

    End_Node (End_Node &&rhs) noexcept : left_{std::exchange (rhs.left_, nullptr)} {}

    End_Node &operator= (End_Node &&rhs) noexcept
    {
        std::swap (left_, rhs.left_);
        return *this;
    }

    virtual ~End_Node() = default;
};

struct BT_Node_Tag {};
struct RB_Node_Tag : public BT_Node_Tag {};
struct Advanced_RB_Node_Tag : public RB_Node_Tag {};

template<typename Key_T, typename Node_Tag>
class Tree_Node : public End_Node<Tree_Node<Key_T, Node_Tag>>
{
    using node_ptr = Tree_Node *;
    using base_ = End_Node<Tree_Node>;

public:

    using key_type = Key_T;

    node_ptr right_ = nullptr;
    node_ptr parent_ = nullptr;

private:

    key_type key_;

public:

    Tree_Node (const key_type &key) : key_{key} {}

    Tree_Node (const Tree_Node &rhs) = delete;
    Tree_Node &operator= (const Tree_Node &rhs) = delete;

    Tree_Node (Tree_Node &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_type>)
            : base_{std::move (rhs)},
              right_{std::exchange (rhs.right, nullptr)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              key_{std::move (rhs.key_)} {}

    Tree_Node &operator= (Tree_Node &rhs) noexcept (std::is_nothrow_move_constructible_v<key_type> &&
                                                    std::is_nothrow_move_assignable_v<key_type>)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (right_, rhs.right_);
        std::swap (parent_, rhs.right_);
        std::swap (key_, rhs.key_);
        
        return *this;
    }

    const key_type &key () const { return key_; }
};

template<typename Key_T>
using BT_Node = Tree_Node<Key_T, BT_Node_Tag>;

enum class RB_Color
{
    red,
    black
};

template<typename Key_T>
class Tree_Node<Key_T, RB_Node_Tag> : public End_Node<Tree_Node<Key_T, RB_Node_Tag>>
{   
    using node_ptr = Tree_Node *;
    using base_ = End_Node<Tree_Node>;

public:

    using key_type = Key_T;
    using color_type = RB_Color;

    node_ptr right_ = nullptr;
    node_ptr parent_ = nullptr;

    color_type color_;

private:

    key_type key_;

public:

    Tree_Node (const key_type &key, color_type color) : color_{color}, key_{key} {}

    Tree_Node (const Tree_Node &rhs) = delete;
    Tree_Node &operator= (const Tree_Node &rhs) = delete;

    Tree_Node (Tree_Node &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_type>)
            : base_{std::move (rhs)},
              right_{std::exchange (rhs.right, nullptr)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              color_{std::move (rhs.color_)},
              key_{std::move (rhs.key_)} {}

    Tree_Node &operator= (Tree_Node &rhs) noexcept (std::is_nothrow_move_constructible_v<key_type> &&
                                                    std::is_nothrow_move_assignable_v<key_type>)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (right_, rhs.right_);
        std::swap (parent_, rhs.right_);
        std::swap (color_, rhs.color_);
        std::swap (key_, rhs.key_);
        
        return *this;
    }

    const key_type &key () const { return key_; }
};

template<typename Key_T>
using RB_Node = Tree_Node<Key_T, RB_Node_Tag>;

template<typename Key_T>
class Tree_Node<Key_T, Advanced_RB_Node_Tag> : public End_Node<Tree_Node<Key_T, Advanced_RB_Node_Tag>>
{
    using node_ptr = Tree_Node *;
    using base_ = End_Node<Tree_Node>;

public:

    using key_type = Key_T;
    using color_type = RB_Color;
    using size_type = std::size_t;

    node_ptr right_ = nullptr;
    node_ptr parent_ = nullptr;

    color_type color_;
    size_type subtree_h_{};

private:

    key_type key_;

public:

    Tree_Node (const key_type &key, color_type color) : color_{color}, key_{key} {}

    Tree_Node (const Tree_Node &rhs) = delete;
    Tree_Node &operator= (const Tree_Node &rhs) = delete;

    Tree_Node (Tree_Node &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_type>)
            : base_{std::move (rhs)},
              right_{std::exchange (rhs.right, nullptr)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              color_{std::move (rhs.color_)},
              subtree_h_{std::exchange (rhs.subtree_h_, 0)},
              key_{std::move (rhs.key_)} {}

    Tree_Node &operator= (Tree_Node &rhs) noexcept (std::is_nothrow_move_constructible_v<key_type> &&
                                                    std::is_nothrow_move_assignable_v<key_type>)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (right_, rhs.right_);
        std::swap (parent_, rhs.right_);
        std::swap (color_, rhs.color_);
        std::swap (subtree_h_, rhs.subtree_h_);
        std::swap (key_, rhs.key_);
        
        return *this;
    }

    const key_type &key () const { return key_; }
};

template<typename Key_T>
using Advanced_RB_Node = Tree_Node<Key_T, Advanced_RB_Node_Tag>;

namespace detail
{

template<typename Node_T>
requires Binary_Tree_Node<Node_T>
bool is_left_child (const Node_T *node) noexcept
{
    assert (node && node->parent_);
    
    return node == node->parent_->left_;
}

template<typename Node_T>
requires Binary_Tree_Node<Node_T>
const Node_T *minimum (const Node_T *node) noexcept
{
    assert (node);
    
    while (node->left_)
        node = node->left_;

    return node;
}

template<typename Node_T>
Node_T *minimum (Node_T *node) noexcept
{
    return const_cast<Node_T *>(minimum (static_cast<const Node_T *>(node)));
}

template<typename Node_T>
requires Binary_Tree_Node<Node_T>
const Node_T *maximum (const Node_T *node) noexcept
{
    assert (node);
    
    while (node->right_)
        node = node->right_;

    return node;
}

template<typename Node_T>
Node_T *maximum (Node_T *node) noexcept
{
    return const_cast<Node_T *>(maximum (static_cast<const Node_T *>(node)));
}

template<typename Node_T>
requires Binary_Tree_Node<Node_T>
const Node_T *successor (const Node_T *node) noexcept
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

template<typename Node_T>
Node_T *successor (Node_T *node) noexcept
{
    return const_cast<Node_T *>(successor (static_cast<const Node_T *>(node)));
}

template<typename Node_T>
requires Binary_Tree_Node<Node_T>
const Node_T *predecessor (const Node_T *node) noexcept
{
    assert (node);
    
    if (node->left_)
        return maximum (node->left_);

    auto parent = node->parent_;
    while (is_left_child (node))
        node = node->parent_;

    return node->parent_;
}

template<typename Node_T>
Node_T *predecessor (Node_T *node) noexcept
{
    return const_cast<Node_T *>(predecessor (static_cast<const Node_T *>(node)));
}

// Sometimes root_ can be affected. So it has to be changed if necessary
template<typename Node_T>
requires Binary_Tree_Node<Node_T>
void left_rotate (Node_T *x)
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
template <typename Node_T>
requires Binary_Tree_Node<Node_T>
void right_rotate (Node_T *x)
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

} // namespace detail

} // namespace yLab

#endif // INCLUDE_NODES_HPP
