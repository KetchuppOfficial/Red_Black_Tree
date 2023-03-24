#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <initializer_list>
#include <memory>
#include <vector>
#include <functional>

#include "nodes.hpp"
#include "tree_iterator.hpp"
#include "details.hpp"

namespace yLab
{

/*
 * Implementation details:
 * 1) root_->parent points to a non-null structure of type End_Node, which has a member
 *    left_ that points back to root_ (end_node)
 */
template <typename Key_T, typename Compare = std::less<Key_T>>
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
    using iterator = tree_iterator<key_type, node_type>;
    using const_iterator = tree_iterator<key_type, const node_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using node_type = RB_Node<key_type>;

private:

    using node_ptr = node_type *;
    using const_node_ptr = const node_type *;
    using end_node_type = End_Node<node_ptr>;
    using end_node_ptr = end_node_type *;
    using u_node_ptr = std::unique_ptr<node_type>;
    using u_end_node_ptr = std::unique_ptr<end_node_type>;

    std::vector<u_node_ptr> nodes_;

    u_end_node_ptr end_node_ = std::make_unique<end_node_type>();

    node_ptr leftmost_  = end_node();
    node_ptr rightmost_ = nullptr;

public:

    RB_Tree () = default;

    template <std::input_iterator it>
    RB_Tree (it first, it last) { insert (first, last); }

    RB_Tree (std::initializer_list<value_type> ilist) { insert (ilist); }

    RB_Tree (const RB_Tree &rhs)
    {
        if (rhs.root())
        {
            auto rhs_node = const_cast<node_ptr>(rhs.root());

            root() = insert_node (rhs_node->key(), rhs_node->color_);
            root()->parent_ = end_node();

            node_ptr node = root();
            while (rhs_node != rhs.end_node())
            {
                if (rhs_node->left_ && node->left_ == nullptr)
                {
                    rhs_node = rhs_node->left_;

                    node->left_ = insert_node (rhs_node->key(), rhs_node->color_);
                    node->left_->parent_ = node;
                    node = node->left_;

                    if (rhs_node == rhs.leftmost_)
                        leftmost_ = node;
                }
                else if (rhs_node->right_ && node->right_ == nullptr)
                {
                    rhs_node = rhs_node->right_;

                    node->right_ = insert_node (rhs_node->key(), rhs_node->color_);
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

    RB_Tree (RB_Tree &&rhs) noexcept
            : nodes_{std::move (rhs.node_)},
              end_node_{std::move (rhs.end_node_)},
              leftmost_{std::exchange (rhs.leftmost_, rhs.end_node())},
              rightmost_{std::exchange (rhs.rightmost_, nullptr)} {}

    RB_Tree &operator= (RB_Tree &&rhs) noexcept
    {
        std::swap (nodes_, rhs.nodes_);
        std::swap (end_node_, rhs.end_node_);
        std::swap (leftmost_, rhs.leftmost_);
        std::swap (rightmost_, rhs.rightmost_);

        return *this;
    }

    ~RB_Tree () = default;

    // Capacity

    auto size () const { return nodes_.size(); }
    bool empty () const { return size() == 0; }

    // Iterators

    auto begin () { return iterator{leftmost_}; }
    auto begin () const { return const_iterator{leftmost_}; }
    auto cbegin () const { return const_iterator{leftmost_}; }

    auto end () { return iterator{end_node()}; }
    auto end () const { return const_iterator{end_node()}; }
    auto cend () const { return const_iterator{end_node()}; }

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
            return {iterator{new_node}, true};
        }
        else
        {
            auto [node, parent] = details::find_v2 (root(), key);
        
            if (node == nullptr) // No node with such key in the tree
            {
                auto new_node = insert_hint_unique (parent, key);
                return {iterator{new_node}, true};
            }
            else
                return {iterator{node}, false};
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
        auto node = details::find (root(), key);
        return (node) ? iterator{node} : end();
    }

    const_iterator find (const key_type &key) const
    {
        auto node = details::find (root(), key);
        return (node) ? const_iterator{node} : cend();
    }

    iterator lower_bound (const key_type &key)
    {
        auto node = details::lower_bound (root(), key);
        return (node) ? iterator{node} : end();
    }

    const_iterator lower_bound (const key_type &key) const
    {
        auto node = details::lower_bound (root(), key);
        return (node) ? const_iterator{node} : cend();
    }

    iterator upper_bound (const key_type &key)
    {
        auto node = details::upper_bound (root(), key);
        return (node) ? iterator{node} : end();
    }

    const_iterator upper_bound (const key_type &key) const
    {
        auto node = details::upper_bound (root(), key);
        return (node) ? const_iterator{node} : cend();
    }

    bool contains (const key_type &key) const { return find (key) != end(); }

private:

    node_ptr end_node () noexcept { return static_cast<node_ptr>(end_node_.get()); }
    const_node_ptr end_node () const noexcept { return static_cast<node_ptr>(end_node_.get()); }

    node_ptr &root () noexcept { return end_node()->left_; }
    const_node_ptr root () const noexcept { return end_node()->left_; }

    node_ptr insert_node (const key_type &key, const RB_Color color)
    {
        u_node_ptr new_node {new node_type{key, color}};
        nodes_.push_back (std::move (new_node));

        return nodes_.back().get();
    }

    node_ptr insert_root (const key_type &key)
    {
        auto new_node = insert_node (key, RB_Color::black);
        
        root() = new_node;
        root()->parent_ = end_node();

        leftmost_ = rightmost_ = new_node;

        return new_node;
    }

    node_ptr insert_hint_unique (node_ptr parent, const key_type &key)
    {
        auto new_node = insert_node (key, RB_Color::red);
        new_node->parent_ = parent;

        if (key < parent->key())
            parent->left_ = new_node;
        else
            parent->right_ = new_node;

        details::rb_insert_fixup (root(), new_node);

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
            auto [node, parent] = details::find_v2 (root(), key);
        
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
        else if (details::is_left_child (u))
            u->parent_->left_ = v;
        else
            u->parent_->right_ = v;

        v->parent_ = u->parent_;
    }
#endif
};

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
