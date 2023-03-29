#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <initializer_list>
#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

#include "nodes.hpp"
#include "tree_iterator.hpp"

#ifdef DEBUG
#include "graphic_dump.hpp"
#endif // DEBUG

namespace yLab
{

namespace detail
{

template <typename Node_T>
auto fixup_subroutine_1 (Node_T *new_node, Node_T *uncle, const Node_T *root)
{
    using color_type = typename Node_T::color_type;

    new_node = new_node->parent_;
    new_node->color_ = color_type::black;

    new_node = new_node->parent_;
    if (new_node != root)
        new_node->color_ = color_type::red;

    uncle->color_ = color_type::black;

    return new_node;
}

template <typename Node_T>
auto fixup_subroutine_2 (Node_T *new_node)
{
    using color_type = typename Node_T::color_type;
    
    new_node = new_node->parent_;
    new_node->color_ = color_type::black;

    new_node = new_node->parent_;
    new_node->color_ = color_type::red;

    return new_node;
}

// RB_invatiant (end_node_->left_) == true
// But end_node_->left_ may be different than the value passed ad root
template <typename Node_T>
void rb_insert_fixup (const Node_T *root, Node_T *new_node)
{       
    using color_type = typename Node_T::color_type;
    
    assert (root && new_node);
    
    // Checks if "The root is black" property violated
    if (new_node == root)
    {
        new_node->color_ = color_type::black;
        return;
    }

    // (new_node != root_) ==> (root_->color_ == color_type::black)

    // Checks if "If a node is red, then both its children are black" property violated
    while (new_node != root && new_node->parent_->color_ == color_type::red)
    {
        // First condition is important only for iterations 2, 3, ... but not for 1
        // (new_node->parent_->color_ == color_type::red) ==> (new_node->parent_ != root_)
        
        if (is_left_child (new_node->parent_))
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            Node_T *uncle = new_node->parent_->parent_->right_;

            if (uncle && uncle->color_ == color_type::red)
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
            Node_T *uncle = new_node->parent_->parent_->left_;

            if (uncle && uncle->color_ == color_type::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (is_left_child (new_node->parent_))
                {
                    new_node = new_node->parent_;
                    detail::right_rotate (new_node);
                }

                left_rotate (fixup_subroutine_2 (new_node));
                break;
            }
        }
    }
}

} // namespace detail

/*
 * Implementation details:
 * 1) root_->parent points to a non-null structure of type End_Node, which has a member
 *    left_ that points back to root_ (end_node)
 */
template <typename Key_T, typename Compare = std::less<Key_T>, typename Node_T = RB_Node<Key_T>>
requires Binary_Tree_Node<Node_T>
class RB_Tree final
{
public:

    using key_type = Key_T;
    using key_compare = Compare;
    using value_type = key_type;
    using value_compare = Compare;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using node_type = Node_T;
    using iterator = tree_iterator<key_type, const node_type>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:

    using color_type = node_type::color_type;
    using node_ptr = node_type *;
    using const_node_ptr = const node_type *;
    using end_node_type = End_Node<node_type>;
    using end_node_ptr = end_node_type *;
    using u_node_ptr = std::unique_ptr<node_type>;
    using u_end_node_ptr = std::unique_ptr<end_node_type>;

    std::vector<u_node_ptr> nodes_;

    u_end_node_ptr end_node_ = std::make_unique<end_node_type>();

    node_ptr leftmost_  = end_node();
    node_ptr rightmost_ = nullptr;

    key_compare comp_;

public:

    RB_Tree () : RB_Tree{key_compare{}} {}

    explicit RB_Tree (const key_compare &comp) : comp_{comp} {}

    template <std::input_iterator it>
    RB_Tree (it first, it last, const key_compare &comp = key_compare{})
    {
        insert (first, last);
    }

    RB_Tree (std::initializer_list<value_type> ilist, const key_compare &comp = key_compare{})
    {
        insert (ilist);
    }

    RB_Tree (const RB_Tree &rhs)
    {
        if (rhs.root())
        {
            auto rhs_node = rhs.root();

            root() = allocate_node (rhs_node->key(), rhs_node->color_);
            root()->parent_ = end_node();

            node_ptr node = root();
            while (rhs_node != rhs.end_node())
            {
                if (rhs_node->left_ && node->left_ == nullptr)
                {
                    rhs_node = rhs_node->left_;

                    node->left_ = allocate_node (rhs_node->key(), rhs_node->color_);
                    node->left_->parent_ = node;
                    node = node->left_;

                    if (rhs_node == rhs.leftmost_)
                        leftmost_ = node;
                }
                else if (rhs_node->right_ && node->right_ == nullptr)
                {
                    rhs_node = rhs_node->right_;

                    node->right_ = allocate_node (rhs_node->key(), rhs_node->color_);
                    node->right_->parent_ = node;
                    node = node->right_;

                    if (rhs_node == rhs.rightmost_)
                        rightmost_ = node;
                }
                else
                {
                    rhs_node = rhs_node->parent_;
                    node = node->parent_;
                }
            }
        }
    }

    RB_Tree &operator= (const RB_Tree &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    RB_Tree (RB_Tree &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_compare>)
            : nodes_{std::move (rhs.node_)},
              end_node_{std::move (rhs.end_node_)},
              leftmost_{std::exchange (rhs.leftmost_, rhs.end_node())},
              rightmost_{std::exchange (rhs.rightmost_, nullptr)} {}

    RB_Tree &operator= (RB_Tree &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_compare> &&
                                                 std::is_nothrow_move_assignable_v<key_compare>)
    {
        std::swap (nodes_, rhs.nodes_);
        std::swap (end_node_, rhs.end_node_);
        std::swap (leftmost_, rhs.leftmost_);
        std::swap (rightmost_, rhs.rightmost_);
        std::swap (comp_, rhs.comp_);

        return *this;
    }

    ~RB_Tree () = default;

    // Observers

    const key_compare &key_comp () const { return comp_; }

    const value_compare &value_comp () const { return key_comp(); }

    // Capacity

    size_type size () const noexcept { return nodes_.size(); }
    bool empty () const noexcept { return size() == 0; }

    // Iterators

    iterator begin () noexcept { return iterator{leftmost_}; }
    const_iterator begin () const noexcept { return const_iterator{leftmost_}; }
    iterator end () { return iterator{end_node()}; }
    const_iterator end () const { return const_iterator{end_node()}; }

    reverse_iterator rbegin () noexcept { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin () const noexcept { return const_reverse_iterator{end()}; }
    reverse_iterator rend () noexcept { return reverse_iterator{begin()}; }
    const_reverse_iterator rend () const noexcept { return const_reverse_iterator{begin()}; }

    const_iterator cbegin () const noexcept { return begin(); }
    const_iterator cend () const noexcept { return end(); }
    const_reverse_iterator crbegin () const noexcept { return rbegin(); }
    const_reverse_iterator crend () const noexcept { return rend(); }

    // Modifiers

    void swap (RB_Tree &other) { std::swap (*this, other); }

    void clear ()
    {
        nodes_.clear();

        leftmost_ = rightmost_ = nullptr;
        root() = nullptr; 
    }

    std::pair<iterator, bool> insert (const key_type &key)
    {
        if (empty())
        {
            auto new_node = insert_root (key);
            return std::make_pair (iterator{new_node}, true);
        }
        else
        {
            auto [node, parent] = find_v2 (root(), key);
        
            if (node == nullptr) // No node with such key in the tree
            {
                auto new_node = insert_hint_unique (parent, key);
                return std::make_pair (iterator{new_node}, true);
            }
            else
                return std::make_pair (iterator{node}, false);
        }
    }

    template<std::input_iterator it>
    void insert (it first, it last)
    {
        for (; first != last; ++first)
            insert_unique (*first);
    }

    void insert (std::initializer_list<value_type> ilist)
    {
        for (auto it = ilist.begin(), end = ilist.end(); it != end; ++it)
            insert_unique (*it);
    }

    // Lookup

    iterator find (const key_type &key)
    {
        auto node = find (root(), key);
        return (node) ? iterator{node} : cend();
    }

    const_iterator find (const key_type &key) const
    {
        return const_cast<RB_Tree &>(*this).find();
    }

    iterator lower_bound (const key_type &key)
    {
        auto node = lower_bound (root(), key);
        return (node) ? iterator{node} : cend();
    }

    const_iterator lower_bound (const key_type &key) const
    {
        return const_cast<RB_Tree &>(*this).lower_bound (key);
    }

    iterator upper_bound (const key_type &key)
    {
        auto node = upper_bound (root(), key);
        return (node) ? iterator{node} : cend();
    }

    const_iterator upper_bound (const key_type &key) const
    {
        return const_cast<RB_Tree &>(*this).upper_bound (key);
    }

    bool contains (const key_type &key) const { return find (key) != end(); }
    
    #ifdef DEBUG

    void graphic_dump (std::ostream &os) { detail::graphic_dump (os, leftmost_, end_node()); }

    #endif // DEBUG

private:

    node_ptr end_node () noexcept { return static_cast<node_ptr>(end_node_.get()); }
    const_node_ptr end_node () const noexcept { return static_cast<node_ptr>(end_node_.get()); }

    node_ptr &root () noexcept { return end_node()->left_; }
    const_node_ptr root () const noexcept { return end_node()->left_; }

    node_ptr find (node_ptr node, const key_type &key)
    {
        while (node && !key_comp() (node->key(), key))
            node = (key < node->key()) ? node->left_ : node->right_;

        return node;
    }

    const_node_ptr find (const_node_ptr node, const key_type &key) const
    {
        return const_cast<RB_Tree &>(*this).find (const_cast<node_ptr>(node), key);
    }

    // (parent == nullptr) ==> (key == root().key())
    // (node != nullptr) ==> (parent != nullptr)
    std::pair<node_ptr, node_ptr> find_v2 (node_ptr node, const key_type &key)
    {
        node_ptr parent = nullptr;

        while (node)
        {
            if (!key_comp() (key, node->key()) && !key_comp() (node->key(), key))
                break;
            
            parent = node;
            node = (key_comp() (key, node->key())) ? node->left_ : node = node->right_;
        }

        return std::make_pair (node, parent);
    }

    // Finds first element that is not less than key
    node_ptr lower_bound (node_ptr node, const key_type &key)
    {    
        node_ptr result = nullptr;
        while (node)
        {
            if (!key_comp() (node->key(), key))
            {
                result = node;
                node = node->left_;
            }
            else
                node = node->right_;
        }

        return result;
    }

    const_node_ptr lower_bound (const_node_ptr node, const key_type &key) const
    {
        return const_cast<RB_Tree &>(*this).lower_bound (const_cast<node_ptr>(node), key);
    }

    // Finds first element that is greater than key
    node_ptr upper_bound (node_ptr node, const key_type &key)
    {
        node_ptr result = nullptr;
        while (node)
        {
            if (key_comp() (key, node->key()))
            {
                result = node;
                node = node->left_;
            }
            else
                node = node->right_;
        }

        return result;
    }

    const_node_ptr upper_bound (const_node_ptr node, const key_type &key) const
    {
        return const_cast<RB_Tree &>(*this).upper_bound (const_cast<node_ptr>(node), key);
    }

    node_ptr allocate_node (const key_type &key, const color_type color)
    {
        u_node_ptr new_node {new node_type{key, color}};
        nodes_.push_back (std::move (new_node));

        return nodes_.back().get();
    }

    node_ptr insert_root (const key_type &key)
    {
        auto new_node = allocate_node (key, color_type::black);
        
        root() = new_node;
        root()->parent_ = end_node();

        leftmost_ = rightmost_ = new_node;

        return new_node;
    }

    node_ptr insert_hint_unique (node_ptr parent, const key_type &key)
    {
        auto new_node = allocate_node (key, color_type::red);
        new_node->parent_ = parent;

        if (key < parent->key())
            parent->left_ = new_node;
        else
            parent->right_ = new_node;

        detail::rb_insert_fixup (root(), new_node);

        if (new_node == leftmost_->left_)
            leftmost_ = new_node;
        else if (new_node == rightmost_->right_)
            rightmost_ = new_node;

        return new_node;
    }

    void insert_unique (const key_type &key)
    {
        if (empty())
            insert_root (key);
        else
        {
            auto [node, parent] = find_v2 (root(), key);
        
            if (node == nullptr)
                insert_hint_unique (parent, key);
        }
    }

#if 0
    // Replaces subtree rooted at node U with the subtree rooted at node V
    void transplant (node_ptr u, node_ptr v)
    {
        if (u->parent_ == end_node())
            root() = v;
        else if (detail::is_left_child (u))
            u->parent_->left_ = v;
        else
            u->parent_->right_ = v;

        v->parent_ = u->parent_;
    }
#endif
};

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
