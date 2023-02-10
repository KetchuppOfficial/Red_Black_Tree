#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <cassert>
#include <tuple>

namespace yLab
{

template <typename Ptr_T>
requires std::is_pointer_v<Ptr_T>
class End_Node
{
    using self = End_Node<Ptr_T>;

public:

    Ptr_T left_ = nullptr;

    End_Node () = default;

    End_Node (const self &rhs) = delete;
    self &operator= (const self &rhs) = delete;

    End_Node (self &&rhs) noexcept : left_{std::exchange (rhs.left_, nullptr)} {}

    self &operator= (self &&rhs) noexcept
    {
        std::swap (left_, rhs.left_);
        return *this;
    }

    virtual ~End_Node() = default;
};

enum class RB_Color
{
    red,
    black
};

template <typename Key_T>
class RB_Node final : public End_Node<RB_Node<Key_T> *>
{
    using self  = RB_Node<Key_T>;
    using base_ = End_Node<self *>;

public:
    
    self *parent_ = nullptr;
    self *right_  = nullptr;

    RB_Color color_;

private:

    Key_T key_;

public:

    RB_Node (Key_T key, RB_Color color) : base_{}, color_{color}, key_{key} {}

    RB_Node (const self &rhs) = delete;
    RB_Node &operator= (const self &rhs) = delete;

    RB_Node (self &&rhs) noexcept 
              : base_ {std::move (rhs)},
                parent_{std::exchange (rhs.parent_, nullptr)},
                right_ {std::exchange (rhs.right_,  nullptr)},
                color_ {std::move (rhs.color_)},
                key_   {std::exchange (rhs.key_, Key_T{})} {}
            
    RB_Node &operator= (self &&rhs) noexcept
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (parent_, rhs.parent_);
        std::swap (right_,  rhs.right_);
        std::swap (key_,    rhs.key_);
        std::swap (color_,  rhs.color_);

        return *this;
    }

    const Key_T &key () const { return key_; }
};

namespace details
{

template<typename Key_T>
bool is_left_child (RB_Node<Key_T> *node) noexcept
{
    assert (node && node->parent_);
    
    return node == node->parent_->left_;
}

template<typename Key_T>
auto minimum (RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    while (node->left_)
        node = node->left_;

    return node;
}

template<typename Key_T>
auto maximum (RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    while (node->right_)
        node = node->right_;

    return node;
}

template<typename Key_T>
auto successor (RB_Node<Key_T> *node) noexcept
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
auto predecessor (RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    if (node->left_)
        return maximum (node->left_);

    auto parent = node->parent;
    while (is_left_child (node))
        node = node->parent_;

    return node->parent_;
}

template <typename Key_T>
auto find (RB_Node<Key_T> *node, const Key_T &key)
{
    assert (node);
    
    while (node && key != node->key())
        node = (key < node->key()) ? node->left_ : node->right_;

    return node;
}

enum class Child_T
{
    left,
    right,
    no_matter
};

// (parent == nullptr) ==> (key == root().key())
// (node != nullptr) ==> (parent != nullptr)
template <typename Key_T>
auto find_v2 (RB_Node<Key_T> *node, const Key_T &key)
{
    using node_ptr = RB_Node<Key_T> *;
    using result = std::tuple<node_ptr, node_ptr, Child_T>;
    
    assert (node);
    
    node_ptr parent = nullptr;
    auto pos = Child_T::no_matter;

    while (node)
    {
        if (key == node->key())
            return result{node, parent, Child_T::no_matter};
        
        parent = node;
        if (key < node->key())
        {
            node = node->left_;
            pos = Child_T::left;
        }
        else
        {
            node = node->right_;
            pos = Child_T::right;
        }
    }

    return result{node, parent, pos};
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
        
        if (details::is_left_child (new_node->parent_))
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            RB_Node<Key_T> *uncle = new_node->parent_->parent_->right_;

            if (uncle && uncle->color_ == RB_Color::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (!details::is_left_child (new_node->parent_))
                {
                    new_node = new_node->parent_;
                    details::left_rotate (new_node);
                }

                details::right_rotate (fixup_subroutine_2 (new_node));
                break;
            }
        }
        else
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            RB_Node<Key_T> * uncle = new_node->parent_->parent_->left_;

            if (uncle && uncle->color_ == RB_Color::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (details::is_left_child (new_node->parent_))
                {
                    new_node = new_node->parent_;
                    details::right_rotate (new_node);
                }

                details::left_rotate (fixup_subroutine_2 (new_node));
                break;
            }
        }
    }
}

} // namespace details

namespace iterators
{

template <typename Key_T>
class tree_iterator final
{
    using node_ptr = RB_Node<Key_T> *;
    using self = tree_iterator;
    
    node_ptr node_;

public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

    tree_iterator () = default;
    tree_iterator (node_ptr node) : node_{node} {}

    reference operator* () const { return node_->key(); }
    pointer operator-> () const { return &node_->key(); }

    self &operator++ ()
    {
        node_ = details::successor (node_);
        return *this;
    }
    
    self operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    self &operator-- ()
    {
        node_ = details::predecessor (node_);
        return *this;
    }

    self operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const self &rhs) const { return node_ == rhs.node_; }
};

template <typename Key_T>
class const_tree_iterator final
{
    using node_ptr = const RB_Node<Key_T> *;
    using self = const_tree_iterator;
    
    node_ptr *node_;

public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = const Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

    const_tree_iterator () = default;
    const_tree_iterator (node_ptr node) : node_{node} {}

    reference operator* () const { return *node_->key(); }
    pointer operator-> () const { return &node_->key(); }

    self &operator++ ()
    {
        node_ = details::successor (node_);
        return *this;
    }
    
    self operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    self &operator-- ()
    {
        node_ = details::predecessor (node_);
        return *this;
    }

    self operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const self &rhs) const { return node_ == rhs.node_; }
};

} // namespace iterators

/*
 * Implementation details:
 * 1) root_->parent points to a non-null structure of type End_Node, which has a member
 *    left_ that points back to root_ (end_node)
 */
template <typename Key_T>
class RB_Tree final
{
    using node_ptr = RB_Node<Key_T> *;
    using const_node_ptr = const RB_Node<Key_T> *;
    using end_node_ptr = End_Node<node_ptr> *;
    using const_end_node_ptr = const End_Node<node_ptr> *;

    end_node_ptr end_node_;

    node_ptr leftmost_  = nullptr;
    node_ptr rightmost_ = nullptr;

    std::size_t size_ = 0;

public:

    using iterator = iterators::tree_iterator<Key_T>;
    using const_iterator = iterators::const_tree_iterator<Key_T>;

    RB_Tree () : end_node_ {new End_Node<node_ptr>} {}

    auto begin () { return iterator{leftmost_}; }
    auto begin () const { return const_iterator{leftmost_}; }
    auto cbegin () const { return const_iterator{leftmost_}; }

    auto end () { return iterator{end_node()}; }
    auto end () const { return const_iterator{end_node()}; }
    auto cend () const { return const_iterator{end_node()}; }

    auto find (const Key_T &key) { return iterator{details::find (key)}; }
    auto find (const Key_T &key) const { return const_iterator{details::find (key)}; }

    std::pair<iterator, bool> insert (const Key_T &key)
    {
        if (size_ == 0) // Tree is empty
        {
            auto new_node = new RB_Node<Key_T>{key, RB_Color::black};

            root() = new_node;
            root()->parent_ = end_node();

            leftmost_ = rightmost_ = new_node;
            size_++;

            return {iterator{new_node}, true};
        }
        else
        {
            auto [node, parent, child_type] = details::find_v2 (root(), key);
        
            if (node == nullptr) // No node with such key in the tree
            {
                auto new_node = new RB_Node<Key_T>{key, RB_Color::red};
                new_node->parent_ = parent;

                if (child_type == details::Child_T::left)
                    parent->left_ = new_node;
                else
                    parent->right_ = new_node;

                details::rb_insert_fixup (root(), new_node);

                if (new_node == leftmost_->left_)
                    leftmost_ = new_node;
                else if (new_node == rightmost_->right_)
                    rightmost_ = new_node;

                size_++;

                return {iterator{new_node}, true};
            }
            else
                return {iterator{node}, false};
        }
    }

private:

    node_ptr &root () noexcept { return end_node_->left_; }
    const_node_ptr root () const noexcept { return end_node_->left_; }

    node_ptr end_node () noexcept { return static_cast<node_ptr>(end_node_); }
    const_node_ptr end_node () const noexcept { return static_cast<node_ptr>(end_node_); }
};

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
