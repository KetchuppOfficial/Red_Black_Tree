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

    using size_type = std::size_t;

    node_ptr left_ = nullptr;
    size_type subtree_size_{1};

    End_Node () = default;

    End_Node (node_ptr left) : left_{left} {}

    End_Node (const End_Node &rhs) = delete;
    End_Node &operator= (const End_Node &rhs) = delete;

    End_Node (End_Node &&rhs) noexcept : left_{std::exchange (rhs.left_, nullptr)},
                                         subtree_size_{std::exchange (rhs.subtree_size_, 1)} {}

    End_Node &operator= (End_Node &&rhs) noexcept
    {
        std::swap (left_, rhs.left_);
        std::swap (subtree_size_, rhs.subtree_size_);
        return *this;
    }

    virtual ~End_Node() = default;
};

// ARB_Node - advanced red-black node
template<typename Key_T>
class ARB_Node : public End_Node<ARB_Node<Key_T>>
{
    using node_ptr = ARB_Node *;
    using base_ = End_Node<ARB_Node>;

public:

    enum class RB_Color
    {
        red,
        black
    };

    using key_type = Key_T;
    using color_type = RB_Color;
    using size_type = std::size_t;

    node_ptr right_ = nullptr;
    node_ptr parent_ = nullptr;

    color_type color_;

private:

    key_type key_;

public:

    ARB_Node (const key_type &key, color_type color) : color_{color}, key_{key} {}

    ARB_Node (const ARB_Node &rhs) = delete;
    ARB_Node &operator= (const ARB_Node &rhs) = delete;

    ARB_Node (ARB_Node &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_type>)
            : base_{std::move (rhs)},
              right_{std::exchange (rhs.right, nullptr)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              color_{std::move (rhs.color_)},
              key_{std::move (rhs.key_)} {}

    ARB_Node &operator= (ARB_Node &rhs) noexcept (std::is_nothrow_move_constructible_v<key_type> &&
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

    auto yl = y->left_;
    x->right_ = yl;
    if (yl)
        yl->parent_ = x;

    auto xp = x->parent_;
    y->parent_ = xp;
    if (is_left_child (x))
        xp->left_ = y;
    else
        xp->right_ = y;

    y->left_ = x;
    x->parent_ = y;
}

template<typename Key_T>
void left_rotate_plus (ARB_Node<Key_T> *x)
{
    auto y = x->right_;

    auto a_size = x->left_->subtree_size_;
    auto b_size = y->left_->subtree_size_;
    auto c_size = y->right_->subtree_size_;

    left_rotate (x);

    auto x_size = a_size + b_size + 1;
    x->subtree_size_ = x_size;
    y->subtree_size_ = x_size + c_size + 1;
}

// Sometimes root_ can be affected. So it has to be changed if necessary
template <typename Node_T>
requires Binary_Tree_Node<Node_T>
void right_rotate (Node_T *x)
{
    assert (x && x->left_);

    auto y = x->left_;

    auto yr = y->right_;
    x->left_ = yr;
    if (yr)
        yr->parent_ = x;

    auto xp = x->parent_;
    y->parent_ = xp;
    if (is_left_child (x))
        xp->left_ = y;
    else
        xp->right_ = y;

    y->right_ = x;
    x->parent_ = y;
}

template<typename Key_T>
void right_rotate_plus (ARB_Node<Key_T> *x)
{
    auto y = x->left_;

    auto a_size = y->left_->subtree_size_;
    auto b_size = y->right_->subtree_size_;
    auto c_size = x->right_->subtree_size_;

    right_rotate (x);

    auto x_size = b_size + c_size + 1;
    x->subtree_size_ = x_size;
    y->subtree_size_ = a_size + x_size + 1;
}

template<typename Key_T>
const ARB_Node<Key_T> *kth_smallest (const ARB_Node<Key_T> *root,
                                     typename ARB_Node<Key_T>::size_type k) noexcept
{
    if (k > root->subtree_size_)
        return nullptr;
    
    auto left = root->left_;
    auto r = left ? left->size_ + 1 : root->size_;
    // (r - 1) - number of nodes in the child subtree of root where kth smallest key locates

    while (k != r)
    {
        if (k < r)
            root = left ? left : root->right_;
        else
        {
            root = root->right_;
            k = k - r;
        }

        left = root->left_;
        r = left ? left->size_ + 1 : root->size_;
    }

    return root;
}

template<typename Key_T>
ARB_Node<Key_T> *kth_smallest (ARB_Node<Key_T> *root,
                               typename ARB_Node<Key_T>::size_type k) noexcept
{
    using node_ptr = ARB_Node<Key_T> *;
    using const_node_ptr = const ARB_Node<Key_T> *;

    return const_cast<node_ptr>(kth_smallest (static_cast<const_node_ptr>(root), k));
}

template<typename Key_T>
auto n_less_than (const ARB_Node<Key_T> *root, const ARB_Node<Key_T> *node) noexcept ->
typename ARB_Node<Key_T>::size_type
{
    using size_type = typename ARB_Node<Key_T>::size_type;
    
    auto left = node->left_;
    auto rank = left ? left->size_ : size_type{0};

    while (node != root)
    {
        if (node == node->parent_->right_)
            rank += node->parent_->left_->size_ + 1;

        node = node->parent_;
    }

    return rank;
}

template<typename Key_T>
auto n_less_than (ARB_Node<Key_T> *root, ARB_Node<Key_T> *node) noexcept ->
typename ARB_Node<Key_T>::size_type
{
    using const_node_ptr = const ARB_Node<Key_T> *;

    return n_less_than (static_cast<const_node_ptr>(root), static_cast<const_node_ptr>(node));
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_NODES_HPP
