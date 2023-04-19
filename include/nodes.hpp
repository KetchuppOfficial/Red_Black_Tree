#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include <type_traits>
#include <utility>
#include <cassert>

namespace yLab
{

// Node representing the result of end()
template<typename Node_T>
class End_Node
{
    using node_ptr = Node_T *;

public:

    using size_type = std::size_t;
    using node_type = Node_T;

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
    using end_node_ptr = base_ *;

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
    end_node_ptr parent_ = nullptr;

    color_type color_;

private:

    key_type key_;

public:

    ARB_Node (const key_type &key, color_type color) : color_{color}, key_{key} {}
    ARB_Node (key_type &&key, color_type color) : color_{color}, key_{key} {}

    ARB_Node (const ARB_Node &rhs) = delete;
    ARB_Node &operator= (const ARB_Node &rhs) = delete;

    ARB_Node (ARB_Node &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_type>)
            : base_{std::move (rhs)},
              right_{std::exchange (rhs.right, nullptr)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              color_{std::move (rhs.color_)},
              key_{std::move (rhs.key_)} {}

    ARB_Node &operator= (ARB_Node &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_type> &&
                                                   std::is_nothrow_move_assignable_v<key_type>)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (right_, rhs.right_);
        std::swap (parent_, rhs.parent_);
        std::swap (color_, rhs.color_);
        std::swap (key_, rhs.key_);
        
        return *this;
    }

    const key_type &key () const { return key_; }

    node_ptr parent_unsafe () const { return static_cast<node_ptr>(parent_); }
};

namespace detail
{

// converts end_node_ptr to node_ptr and const_end_node_ptr to const_node_ptr
template<typename End_Node_Ptr>
class downcast
{
    using end_node_type = std::remove_reference_t<decltype (*std::declval<End_Node_Ptr>())>;
    using node_type = typename end_node_type::node_type;

public:

    using type = std::conditional_t<std::is_const_v<end_node_type>, const node_type *, node_type *>;
};

template<typename End_Node_Ptr>
using downcast_t = typename downcast<End_Node_Ptr>::type;

// converts node_ptr to end_node_ptr and const_node_ptr to const_end_node_ptr
template<typename Node_Ptr>
class upcast
{
    using node_type = std::remove_reference_t<decltype (*std::declval<Node_Ptr>())>;
    using end_node_type = yLab::End_Node<std::remove_cv_t<node_type>>;

public:

    using type = std::conditional_t<std::is_const_v<node_type>, const end_node_type *, end_node_type *>;
};

template<typename Node_Ptr>
using upcast_t = typename upcast<Node_Ptr>::type;

template<typename Node_T>
bool is_left_child (const Node_T *node) noexcept
{
    assert (node && node->parent_);
    
    return node == node->parent_->left_;
}

template<typename Node_Ptr>
Node_Ptr minimum (Node_Ptr node) noexcept
{
    assert (node);
    
    while (node->left_)
        node = node->left_;

    return node;
}

template<typename Node_Ptr>
Node_Ptr maximum (Node_Ptr node) noexcept
{
    assert (node);
    
    while (node->right_)
        node = node->right_;

    return node;
}

template<typename Node_Ptr>
upcast_t<Node_Ptr> successor (Node_Ptr node) noexcept
{
    assert (node);
    
    if (node->right_)
        return minimum (node->right_);

    while (!is_left_child (node))
        node = node->parent_unsafe();
    
    return node->parent_;
}

template<typename End_Node_Ptr>
downcast_t<End_Node_Ptr> predecessor (End_Node_Ptr node) noexcept
{
    assert (node);
    
    if (node->left_)
        return maximum (node->left_);

    auto node_ = static_cast<downcast_t<End_Node_Ptr>>(node);
    while (is_left_child (node_))
        node_ = node_->parent_unsafe();

    return node_->parent_unsafe();
}

template<typename Node_T>
void left_rotate (Node_T *x) noexcept
{
    assert (x && x->right_);

    auto y = x->right_;

    auto yl = y->left_;
    x->right_ = yl;
    if (yl)
        yl->parent_ = x;

    y->parent_ = x->parent_;
    if (is_left_child (x))
        x->parent_->left_ = y;
    else
        x->parent_unsafe()->right_ = y;

    y->left_ = x;
    x->parent_ = y;
}

template<typename Key_T>
void left_rotate_plus (ARB_Node<Key_T> *x) noexcept
{
    using size_type = typename ARB_Node<Key_T>::size_type;
    
    assert (x && x->right_);
    
    auto y = x->right_;

    auto a_size = x->subtree_size_ - y->subtree_size_ - 1;
    auto b_size = (y->left_) ? y->left_->subtree_size_ : size_type{0};
    auto c_size = (y->right_) ? y->right_->subtree_size_ : size_type{0};

    left_rotate (x);

    auto x_size = a_size + b_size + 1;
    x->subtree_size_ = x_size;
    y->subtree_size_ = x_size + c_size + 1;
}

template <typename Node_T>
void right_rotate (Node_T *x) noexcept
{
    assert (x && x->left_);

    auto y = x->left_;

    auto yr = y->right_;
    x->left_ = yr;
    if (yr)
        yr->parent_ = x;

    y->parent_ = x->parent_;
    if (is_left_child (x))
        x->parent_->left_ = y;
    else
        x->parent_unsafe()->right_ = y;

    y->right_ = x;
    x->parent_ = y;
}

template<typename Key_T>
void right_rotate_plus (ARB_Node<Key_T> *x) noexcept
{
    using size_type = typename ARB_Node<Key_T>::size_type;
    
    assert (x && x->left_);
    
    auto y = x->left_;

    auto c_size = x->subtree_size_ - y->subtree_size_ - 1;
    auto a_size = (y->left_) ? y->left_->subtree_size_ : size_type{0};
    auto b_size = (y->right_) ? y->right_->subtree_size_ : size_type{0};

    right_rotate (x);

    auto x_size = b_size + c_size + 1;
    x->subtree_size_ = x_size;
    y->subtree_size_ = a_size + x_size + 1;
}

template<typename Node_Ptr>
Node_Ptr kth_smallest (Node_Ptr root, std::size_t k) noexcept
{
    using size_type = std::remove_pointer_t<Node_Ptr>::size_type;
    
    if (k > root->subtree_size_)
        return nullptr;

    auto left = root->left_;
    auto left_size = (left) ? left->subtree_size_ : size_type{0};
    
    while (k != left_size + 1)
    {    
        if (0 < left_size && k <= left_size)
            root = left;
        else
        {
            root = root->right_;
            k -= (left_size + 1);
        }

        left = root->left_;
        left_size = (left) ? left->subtree_size_ : size_type{0};
    }

    return root;
}

template<typename End_Node_Ptr>
auto n_less_than (End_Node_Ptr root, End_Node_Ptr node) noexcept ->
typename std::remove_pointer_t<End_Node_Ptr>::size_type
{
    using size_type = std::remove_pointer_t<End_Node_Ptr>::size_type;
    
    auto left = node->left_;
    auto rank = left ? left->subtree_size_ : size_type{0};

    while (node != root)
    {
        auto node_ = static_cast<downcast_t<End_Node_Ptr>>(node);
        auto np = node_->parent_;

        if (!is_left_child (node_))
            rank += (np->left_) ? np->left_->subtree_size_ + 1 : 1;

        node = node_->parent_;
    }

    return rank;
}

template<typename Node_T>
std::size_t red_black_verifier (const Node_T *root) noexcept
{
    using color_type = typename Node_T::color_type;
    
    if (root == nullptr)
        return 1;

    if ((root->left_ && root->left_->parent_ != root) &&
        (root->right_ && root->right_->parent_ != root))
        return 0;

    if (root->color_ == color_type::red)
    {
        if ((root->left_  && root->left_->color_  == color_type::red) ||
            (root->right_ && root->right_->color_ == color_type::red))
            return 0;
    }

    auto left_black_height = red_black_verifier (root->left_);
    if (left_black_height == 0)
        return 0;
    if (left_black_height != red_black_verifier (root->right_))
        return 0;

    return left_black_height + ((root->color_ == color_type::black) ? 1 : 0);
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_NODES_HPP
