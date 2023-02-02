#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <iterator>

namespace yLab
{

enum class RB_Color
{
    red,
    black
};

template <typename Key_T>
class RB_Node final
{  
    RB_Node *parent_ = nullptr;
    RB_Node *left_   = nullptr;
    RB_Node *right_  = nullptr;

    RB_Color color_;

    Key_T key_;

public:

    RB_Node (Key_T key, RB_Color color) : key_{key}, color_{color} {}

    RB_Node (const RB_Node &rhs) = delete;
    RB_Node &operator= (const RB_Node &rhs) = delete;

    RB_Node (RB_Node &&rhs) noexcept
            : parent_{std::exchange (rhs.parent_, nullptr)},
              left_  {std::exchange (rhs.left_, nullptr)},
              right_ {std::exchange (rhs.right_, nullptr)},
              color_ {std::move (rhs.color_)},
              key_   {std::move (rhs.key_)} {}
            
    RB_Node &operator= (RB_Node &&rhs) noexcept
    {
        std::swap (parent_, rhs.parent_);
        std::swap (left_, rhs.left_);
        std::swap (right_, rhs.right_);
        std::swap (color_, rhs.color_);
        std::swap (key_, rhs.key_);

        return *this;
    }

    ~RB_Node () = default;

    RB_Node *minimum () const noexcept
    {
        auto min = this;
        while (min->left_)
            min = min->left_;

        return min;
    }

    RB_Node *maximum () const noexcept
    {
        auto max = this;
        while (max->right_)
            max = max->right_;

        return max;
    }

    RB_Node *successor () const noexcept
    {
        auto curr = this;
        if (curr->right_)
            return curr->right_->minimum();

        auto parent = curr->parent_;
        while (parent && curr == parent->right_)
        {
            curr = parent;
            parent = parent->parent_;
        }

        return parent;
    }

    RB_Node *predecessor () const noexcept
    {
        auto curr = this;
        if (curr->left_)
            return curr->left_->maximum();

        auto parent = curr->parent;
        while (parent && curr == parent->left_)
        {
            curr = parent;
            parent = parent->parent_;
        }

        return parent;
    }
};

template <typename Key_T>
class RB_Iterator final
{
    using node_ptr_t = RB_Node<Key_T> *;
    
    node_ptr_t *node_;

public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Key_T;
    using reference = Key_T&;
    using pointer = Key_T*;

    RB_Iterator () = default;
    RB_Iterator (node_ptr_t node) : node_{node} {}

    reference operator* () const { return *node_; }
    pointer operator-> () const { return node_; }

    RB_Iterator &operator++ ()
    {
        node_ = node_->successor();
        return *this;
    }
    
    RB_Iterator operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    RB_Iterator &operator-- ()
    {
        node_ = node_->predecessor();
        return *this;
    }

    RB_Iterator operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const RB_Iterator &rhs) const { return node_ == rhs.node_; }
};

template <typename Key_T>
class const_RB_Iterator final
{
    using node_ptr_t = const RB_Node<Key_T> *;
    
    node_ptr_t *node_;

public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = const Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

    const_RB_Iterator () = default;
    const_RB_Iterator (node_ptr_t node) : node_{node} {}

    reference operator* () const { return *node_; }
    pointer operator-> () const { return node_; }

    const_RB_Iterator &operator++ ()
    {
        node_ = node_->successor();
        return *this;
    }
    
    const_RB_Iterator operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    const_RB_Iterator &operator-- ()
    {
        node_ = node_->predecessor();
        return *this;
    }

    const_RB_Iterator operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const const_RB_Iterator &rhs) const { return node_ == rhs.node_; }
};

template <typename Key_T>
class RB_Tree final
{
    using node_ptr_t = RB_Node<Key_T> *;

    node_ptr_t top_ = nullptr;
    node_ptr_t leftmost_ = nullptr;
    node_ptr_t rightmost_ = nullptr;

public:

    RB_Tree () = default;

    using iterator = RB_Iterator<Key_T>;
    using const_iterator = const_RB_Iterator<Key_T>;

    auto begin () { return iterator{leftmost_}; }
    auto begin () const { return const_iterator{leftmost_}; }
    auto cbegin () const { return const_iterator{leftmost_}; } 

    auto end () { return iterator{rightmost_}; }
    auto end () const { return const_iterator{rightmost_}; }
    auto cend () const { return const_iterator{rightmost_}; } 
};

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
