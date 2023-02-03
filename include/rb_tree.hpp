#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <iterator>

namespace yLab
{

template <typename Key_T>
struct Search_Node
{  
    Search_Node *parent_ = nullptr;
    Search_Node *left_   = nullptr;
    Search_Node *right_  = nullptr;

protected:

    Key_T key_;

public:

    Search_Node (Key_T key) : key_{key} {}

    Search_Node (const Search_Node &rhs) = delete;
    Search_Node &operator= (const Search_Node &rhs) = delete;

    Search_Node (Search_Node &&rhs) noexcept
                : parent_{std::exchange (rhs.parent_, nullptr)},
                  left_  {std::exchange (rhs.left_, nullptr)},
                  right_ {std::exchange (rhs.right_, nullptr)},
                  key_   {std::move (rhs.key_)} {}
            
    Search_Node &operator= (Search_Node &&rhs) noexcept
    {
        std::swap (parent_, rhs.parent_);
        std::swap (left_, rhs.left_);
        std::swap (right_, rhs.right_);
        std::swap (key_, rhs.key_);

        return *this;
    }

    virtual ~Search_Node () = default;

    Search_Node *minimum () const noexcept
    {
        auto min = this;
        while (min->left_)
            min = min->left_;

        return min;
    }

    Search_Node *maximum () const noexcept
    {
        auto max = this;
        while (max->right_)
            max = max->right_;

        return max;
    }

    Search_Node *successor () const noexcept
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

    Search_Node *predecessor () const noexcept
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
class Search_Iterator final
{
    using node_ptr_t = Search_Node<Key_T> *;
    using self_t = Search_Iterator;
    
    node_ptr_t *node_;

public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Key_T;
    using reference = Key_T&;
    using pointer = Key_T*;

    Search_Iterator () = default;
    Search_Iterator (node_ptr_t node) : node_{node} {}

    reference operator* () const { return *node_; }
    pointer operator-> () const { return node_; }

    self_t &operator++ ()
    {
        node_ = node_->successor();
        return *this;
    }
    
    self_t operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    self_t &operator-- ()
    {
        node_ = node_->predecessor();
        return *this;
    }

    self_t operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const self_t &rhs) const { return node_ == rhs.node_; }
};

template <typename Key_T>
class const_Search_Iterator final
{
    using node_ptr_t = const Search_Node<Key_T> *;
    using self_t = const_Search_Iterator;
    
    node_ptr_t *node_;

public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = const Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

    const_Search_Iterator () = default;
    const_Search_Iterator (node_ptr_t node) : node_{node} {}

    reference operator* () const { return *node_; }
    pointer operator-> () const { return node_; }

    self_t &operator++ ()
    {
        node_ = node_->successor();
        return *this;
    }
    
    self_t operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    self_t &operator-- ()
    {
        node_ = node_->predecessor();
        return *this;
    }

    self_t operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const self_t &rhs) const { return node_ == rhs.node_; }
};

enum class RB_Color
{
    red,
    black
};

template <typename Key_T>
class RB_Node final : public Search_Node<Key_T>
{
    RB_Color color_;

public:

    RB_Node (Key_T key, RB_Color color) : Search_Node<Key_T>{key}, color_{color} {}

    RB_Node (const RB_Node &rhs) = delete;
    RB_Node &operator= (const RB_Node &rhs) = delete;

    RB_Node (RB_Node &&rhs) = default;
    RB_Node &operator= (RB_Node &&rhs) = default;
};

template <typename Key_T>
class RB_Tree final
{
    using node_ptr_t = RB_Node<Key_T> *;

    node_ptr_t root_ = nullptr;
    node_ptr_t leftmost_ = nullptr;
    node_ptr_t rightmost_ = nullptr;

public:

    RB_Tree () = default;

    using iterator = Search_Iterator<Key_T>;
    using const_iterator = const_Search_Iterator<Key_T>;

    auto begin () { return iterator{leftmost_}; }
    auto begin () const { return const_iterator{leftmost_}; }
    auto cbegin () const { return const_iterator{leftmost_}; } 

    auto end () { return iterator{nullptr}; }
    auto end () const { return const_iterator{nullptr}; }
    auto cend () const { return const_iterator{nullptr}; } 

private:

    void left_rotate (node_ptr_t l_node)
    {
        auto r_node = l_node->right;

        l_node->right_ = r_node->left_;
        if (r_node->left_)
            r_node->left_->parent_ = l_node;

        r_node->parent_ = l_node->parent_;
        if (l_node->parent_)
            root_ = r_node;
        else if (l_node == l_node->parent_->left_)
            l_node->parent_->left_ = r_node;
        else
            l_node->parent_->right_ = r_node;

        r_node->left_ = l_node;
        l_node->parent_ = r_node;
    }

    void right_rotate (node_ptr_t r_node)
    {
        auto l_node = r_node->left_;

        r_node->left_ = l_node->right_;
        if (l_node->right_)
            l_node->right->parent_ = r_node;

        l_node->parent_ = r_node->parent_;
        if (r_node->parent_)
            root_ = l_node;
        else if (r_node == r_node->parent_->left_)
            r_node->parent_->left_ = l_node;
        else
            r_node->parent_->right_ = l_node;

        l_node->right_ = r_node;
        r_node->parent_ = l_node;
    }
};

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
