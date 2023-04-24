#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <initializer_list>
#include <functional>
#include <type_traits>
#include <cassert>
#include <compare>

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
void rb_insert_fixup (const Node_T *root, Node_T *new_node)
{       
    using color_type = typename Node_T::color_type;

    auto fixup_subroutine_1 = [](Node_T *new_node, Node_T *uncle, const Node_T *root)
    {
        new_node = new_node->parent_unsafe();
        new_node->color_ = color_type::black;

        new_node = new_node->parent_unsafe();
        new_node->color_ = (new_node == root) ? color_type::black : color_type::red;

        uncle->color_ = color_type::black;

        return new_node;
    };

    auto fixup_subroutine_2 = [](Node_T *new_node)
    {        
        new_node = new_node->parent_unsafe();
        new_node->color_ = color_type::black;

        new_node = new_node->parent_unsafe();
        new_node->color_ = color_type::red;

        return new_node;
    };
    
    assert (root && new_node);
    
    // Checks if "The root is black" property violated
    if (new_node == root)
    {
        new_node->color_ = color_type::black;
        return;
    }

    // Further: (new_node != root_) ==> (root_->color_ == color_type::black)

    // Checks if "If a node is red, then both its children are black" property violated
    while (new_node != root && new_node->parent_unsafe()->color_ == color_type::red)
    {
        // First condition is important only for iterations 2, 3, ... but not for 1
        // (new_node != root) ==> exists (new_node->parent_)
        // (new_node->parent_->color_ == color_type::red) ==> (new_node->parent_ != root_)
        
        if (is_left_child (new_node->parent_unsafe()))
        {
            // (new_node->parent_ != root_) ==> exists (new_node->parent_->parent_) != end_node
            auto uncle = new_node->parent_unsafe()->parent_unsafe()->get_right();

            if (uncle && uncle->color_ == color_type::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (!is_left_child (new_node))
                {
                    new_node = new_node->parent_unsafe();
                    left_rotate_plus (new_node);
                }

                right_rotate_plus (fixup_subroutine_2 (new_node));
                break;
            }
        }
        else
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            auto uncle = new_node->parent_unsafe()->get_parent()->get_left();

            if (uncle && uncle->color_ == color_type::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (is_left_child (new_node))
                {
                    new_node = new_node->parent_unsafe();
                    right_rotate_plus (new_node);
                }

                left_rotate_plus (fixup_subroutine_2 (new_node));
                break;
            }
        }
    }
}

template<typename Node_T>
void rb_erase_fixup (Node_T *root, Node_T *x, Node_T *w)
{
    using color_type = typename Node_T::color_type;
    
    if (x)
        x->color_ = color_type::black;
    else
    {
        while (true)
        {
            if (!is_left_child (w))
            {
                if (w->color_ == color_type::red)
                {
                    auto wp = w->parent_unsafe();

                    w->color_ = color_type::black;
                    wp->color_ = color_type::red;
                    left_rotate_plus (wp);

                    auto wl = w->get_left();
                    if (root == wl)
                        root = w;

                    w = wl->get_right();
                }

                auto wl = w->get_left();
                auto wr = w->get_right();
                if ((wl == nullptr || wl->color_ == color_type::black) &&
                    (wr == nullptr || wr->color_ == color_type::black))
                {
                    w->color_ = color_type::red;
                    x = w->parent_unsafe();

                    if (x == root || x->color_ == color_type::red)
                    {
                        x->color_ = color_type::black;
                        break;
                    }

                    w = is_left_child (x) ? x->parent_unsafe()->get_right()
                                          : x->get_parent()->get_left();
                }
                else
                {
                    if (wr == nullptr || wr->color_ == color_type::black)
                    {
                        wl->color_ = color_type::black;
                        w->color_ = color_type::red;
                        right_rotate_plus (w);
                        w = w->parent_unsafe();
                    }

                    auto wp = w->parent_unsafe();
                    w->color_ = wp->color_;
                    wp->color_ = color_type::black;
                    wr->color_ = color_type::black;
                    left_rotate_plus (wp);
                    break;
                }
            }
            else
            {
                if (w->color_ == color_type::red)
                {
                    auto wp = w->parent_unsafe();

                    w->color_ = color_type::black;
                    wp->color_ = color_type::red;
                    right_rotate_plus (wp);

                    auto wr = w->get_right();
                    if (root == wr)
                        root = w;

                    w = wr->get_left();
                }

                auto wl = w->get_left();
                auto wr = w->get_right();
                if ((wl == nullptr || wl->color_ == color_type::black) &&
                    (wr == nullptr || wr->color_ == color_type::black))
                {
                    w->color_ = color_type::red;
                    x = w->parent_unsafe();

                    if (x == root || x->color_ == color_type::red)
                    {
                        x->color_ = color_type::black;
                        break;
                    }

                    w = is_left_child (x) ? x->parent_unsafe()->get_right()
                                          : x->parent_unsafe()->get_left();
                }
                else
                {
                    if (wl == nullptr || wl->color_ == color_type::black)
                    {
                        wr->color_ = color_type::black;
                        w->color_ = color_type::red;
                        left_rotate_plus (w);
                        w = w->parent_unsafe();
                    }

                    auto wp = w->parent_unsafe();

                    w->color_ = wp->color_;
                    wp->color_ = color_type::black;
                    wl->color_ = color_type::black;
                    right_rotate_plus (wp);
                    break;
                }
            }
        }
    }
}

template<typename Node_T>
void erase (Node_T *root, Node_T *z) noexcept
{    
    using color_type = typename Node_T::color_type;
    
    assert (root && z);
    
    // if (z == maximum (root)) then z->get_right() == nullptr ==> y = z
    // else exists successor (z) != end_node
    auto y = (z->get_left() && z->get_right()) ? static_cast<Node_T *>(successor (z)) : z;

    // (x == nullptr) <==> (y->get_left() == nullptr) && (y->get_right() == nullptr)
    auto x = y->get_left() ? y->get_left() : y->get_right();
    Node_T *w = nullptr;

    // root may change its value further, so we save it
    auto end_node = root->get_parent();

    if (is_left_child (y))
    {
        // STATEMENT (*)
        // Let y == successor (z), then:
        // is_left_child (y) ==> (y != z->get_right()) ==> (y->parent_ != z)
        // Even if (x == nullptr), then (z->get_left() != nullptr)
        y->get_parent()->set_left (x);
        
        if (y != root)
        {
            // y != root ==> exists y->parent_ != end_node
            w = y->parent_unsafe()->get_right();
        }   
        else
            root = x;
    }
    else
    {
        // y is its parent's right child ==> exists y->parent != end_node
        auto yp = y->parent_unsafe();

        // if (yp == z) then following line may set z->get_right() to nullptr
        yp->set_right (x);
        w = yp->get_left();
    }

    // Next if-statement may change z->subtree_size_, so we save it
    auto z_size = z->subtree_size_;

    if (auto yp = y->get_parent(); x != nullptr)
    {
        x->set_parent (yp);
        yp->subtree_size_ -= (y->subtree_size_ - x->subtree_size_);
    }
    else
        yp->subtree_size_ -= y->subtree_size_;

    auto y_orig_color = y->color_;

    if (y != z)
    {
        if (auto yp = y->parent_unsafe(); yp != z)
        {
            auto dec = (x) ? y->subtree_size_ - x->subtree_size_ : y->subtree_size_;
            for (auto ypp = yp->parent_unsafe(); ypp != z; ypp = ypp->parent_unsafe())
                ypp->subtree_size_ -= dec;
        }
        
        auto zl = z->get_left();
        auto zr = z->get_right();

        // We are sure that zl != nullptr because of (*)
        zl->set_parent (y);
        if (zr)
            zr->set_parent (y);

        y->set_left (zl);
        y->set_right (zr);

        y->subtree_size_ = (zl ? zl->subtree_size_ : 0) +
                           (zr ? zr->subtree_size_ : 0) + 1;

        y->color_ = z->color_;

        if (is_left_child (z))
        {
            auto zp = z->get_parent();
            zp->set_left (y);
            zp->subtree_size_ += (y->subtree_size_ - z_size);
        }
        else
        {
            auto zp = z->parent_unsafe();
            zp->set_right (y);
            zp->subtree_size_ += (y->subtree_size_ - z_size);
        }

        y->set_parent (z->get_parent());

        if (z == root)
            root = y;
    }

    if (z->get_parent() != end_node)
    {
        auto zp = z->parent_unsafe();
        for (auto node = zp->get_parent(); node != end_node;
             node = static_cast<Node_T *>(node)->get_parent())
        {
            node->subtree_size_--;
        }
        end_node->subtree_size_--;
    }

    if (y_orig_color == color_type::black && root)
        rb_erase_fixup (root, x, w);
}

} // namespace detail

/*
 * Implementation details:
 * 1) root_->parent points to a non-null structure of type End_Node, which has a member
 *    left_ that points back to root_ (end_node)
 */
template <typename Key_T, typename Compare = std::less<Key_T>>
class ARB_Tree final
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
    using node_type = ARB_Node<key_type>;
    using const_iterator = tree_iterator<node_type>;
    using iterator = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:

    using color_type = typename node_type::color_type;
    using node_ptr = node_type *;
    using const_node_ptr = const node_type *;
    using end_node_type = End_Node<node_type>;
    using end_node_ptr = end_node_type *;
    using const_end_node_ptr = const end_node_type *;

    struct Root_Wrapper
    {
        end_node_type end_node_{};

        Root_Wrapper () = default;

        Root_Wrapper (const Root_Wrapper &rhs) = delete;
        Root_Wrapper &operator= (const Root_Wrapper &rhs) = delete;

        Root_Wrapper (Root_Wrapper &&rhs) noexcept (std::is_nothrow_constructible_v<end_node_type>)
                     : end_node_{std::move (rhs.end_node_)} {}

        Root_Wrapper &operator= (Root_Wrapper &&rhs) noexcept (std::is_nothrow_swappable_v<end_node_type>)
        {
            std::swap (end_node_, rhs.end_node_);

            return *this;
        }

        void clean_up ()
        {
            for (node_ptr node = end_node_.get_left(), save{}; node != nullptr; node = save)
            {
                if (node->get_left() == nullptr)
                {
                    save = node->get_right();
                    delete node;
                }
                else
                {
                    save = node->get_left();
                    node->set_left (save->get_right());
                    save->set_right (node);
                }
            }
        }

        ~Root_Wrapper () { clean_up(); }
    };

    Root_Wrapper root_{};
    const_end_node_ptr leftmost_  = std::addressof (end_node());
    key_compare comp_;

public:

    ARB_Tree () : ARB_Tree{key_compare{}} {}

    explicit ARB_Tree (const key_compare &comp) : comp_{comp} {}

    template<std::input_iterator it>
    ARB_Tree (it first, it last, const key_compare &comp = key_compare{}) : comp_{comp}
    {
        insert (first, last);
    }

    ARB_Tree (std::initializer_list<value_type> ilist, const key_compare &comp = key_compare{}) 
            : comp_{comp}
    {
        insert (ilist);
    }

    ARB_Tree (const ARB_Tree &rhs) : comp_{rhs.comp_}
    {
        for (auto &&key : rhs)
            insert_unique (key);
    }

    ARB_Tree &operator= (const ARB_Tree &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    ARB_Tree (ARB_Tree &&rhs) noexcept (std::is_nothrow_move_constructible_v<key_compare>)
             : root_{std::move (rhs.root_)},
               leftmost_{std::exchange (rhs.leftmost_, std::addressof (rhs.end_node()))},
               comp_{std::move (rhs.comp_)} {}

    ARB_Tree &operator= (ARB_Tree &&rhs) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                                   std::is_nothrow_swappable_v<end_node_type>)
    {
        swap (rhs);

        return *this;
    }

    ~ARB_Tree () = default;

    // Observers

    const key_compare &key_comp () const { return comp_; }

    const value_compare &value_comp () const { return key_comp(); }

    // Capacity

    size_type size () const noexcept { return end_node().subtree_size_ - 1; }
    bool empty () const noexcept { return size() == 0; }

    // Iterators

    iterator begin () noexcept { return iterator{leftmost_}; }
    const_iterator begin () const noexcept { return const_iterator{leftmost_}; }
    iterator end () { return iterator{std::addressof (end_node())}; }
    const_iterator end () const { return const_iterator{std::addressof (end_node())}; }

    reverse_iterator rbegin () noexcept { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin () const noexcept { return const_reverse_iterator{end()}; }
    reverse_iterator rend () noexcept { return reverse_iterator{begin()}; }
    const_reverse_iterator rend () const noexcept { return const_reverse_iterator{begin()}; }

    const_iterator cbegin () const noexcept { return begin(); }
    const_iterator cend () const noexcept { return end(); }
    const_reverse_iterator crbegin () const noexcept { return rbegin(); }
    const_reverse_iterator crend () const noexcept { return rend(); }

    // Modifiers

    void swap (ARB_Tree &other) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                         std::is_nothrow_swappable_v<end_node_type>)
    {
        std::swap (root_, other.root_);
        std::swap (leftmost_, other.leftmost_);
        std::swap (comp_, other.comp_);
    }

    void clear ()
    {
        root_.clean_up ();

        leftmost_ = std::addressof (end_node());
        set_root (nullptr);

        end_node().subtree_size_ = 1;
    }

    std::pair<iterator, bool> insert (const key_type &key)
    {
        auto [node, parent, side] = find_v2 (get_root(), key);
    
        if (node == nullptr) // No node with such key in the tree
        {
            auto new_node = insert_hint_unique (key, parent ? parent : std::addressof (end_node()), side);
            return std::make_pair (iterator{new_node}, true);
        }
        else
            return std::make_pair (iterator{node}, false);
    }

    template<std::input_iterator it>
    void insert (it first, it last)
    {
        for (; first != last; ++first)
            insert_unique (*first);
    }

    void insert (std::initializer_list<value_type> ilist)
    {
        for (auto &&key : ilist)
            insert_unique (key);
    }

    iterator erase (iterator pos)
    {
        auto node = const_cast<end_node_ptr>(pos.node_);
        ++pos;

        if (node == leftmost_)
            leftmost_ = pos.node_;

        detail::erase (get_root(), static_cast<node_ptr>(node));
        delete node;

        assert (search_verifier());
        assert (red_black_verifier());
        assert (subtree_sizes_verifier());

        return pos;
    }

    size_type erase (const key_type &key)
    {
        auto it = find (key);
        if (it == end())
            return 0;
        else
        {
            erase (it);
            return 1;
        }
    }

    // Lookup

    const_iterator find (const key_type &key) const
    {
        auto node = find (get_root(), key);
        return (node) ? const_iterator{node} : end();
    }

    iterator find (const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).find (key);
    }

    const_iterator lower_bound (const key_type &key) const
    {
        auto node = lower_bound (get_root(), key);
        return (node) ? const_iterator{node} : end();
    }

    iterator lower_bound (const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).lower_bound (key);
    }

    const_iterator upper_bound (const key_type &key) const
    {
        auto node = upper_bound (get_root(), key);
        return (node) ? const_iterator{node} : end();
    }

    iterator upper_bound (const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).upper_bound (key);
    }

    bool contains (const key_type &key) const { return find (key) != end(); }

    const_iterator kth_smallest (size_type k) const
    {
        assert (subtree_sizes_verifier ());

        if (empty() || k == 0)
            return end();
        
        auto node = detail::kth_smallest (get_root(), k);
        return (node) ? const_iterator{node} : end();
    }

    iterator kth_smallest (size_type k)
    {
        return static_cast<const ARB_Tree &>(*this).kth_smallest (k);
    }

    size_type n_less_than (const key_type &key) const
    {
        assert (subtree_sizes_verifier ());
        
        if (empty())
            return 0;

        auto it = lower_bound (key);
        if (it == end())
            return size();
        else
            return detail::n_less_than<const_end_node_ptr> (get_root(), it.node_);
    }

    #ifdef DEBUG
    
    // I see how this violates SRP but I don't know any better implementation
    void graphic_dump (std::ostream &os) const
    {
        if (empty())
            return;

        detail::graphic_dump (os, static_cast<const_node_ptr>(leftmost_), std::addressof (end_node()));
    }

    #endif // DEBUG

private:

    end_node_type &end_node () noexcept { return root_.end_node_; }
    const end_node_type &end_node () const noexcept { return root_.end_node_; }

    void set_root (node_ptr new_root) noexcept { return end_node().set_left (new_root); }
    const_node_ptr get_root () const noexcept { return end_node().get_left(); }
    node_ptr get_root () noexcept { return end_node().get_left(); }

    const_node_ptr find (const_node_ptr node, const key_type &key) const
    {
        while (node && (comp_(key, node->key()) || comp_(node->key(), key)))
            node = comp_(key, node->key()) ? node->get_left() : node->get_right();

        return node;
    }

    node_ptr find (node_ptr node, const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).find (node, key);
    }

    enum class Side { left, right };

    // No need for const overload as find_v2 is used only in insert
    std::tuple<node_ptr, end_node_ptr, Side> find_v2 (node_ptr node, const key_type &key)
    {
        // (parent == nullptr) ==> (key == get_root().key())
        // (node != nullptr) ==> (parent != nullptr)

        end_node_ptr parent = nullptr;
        Side side = Side::left; // root is a left child of end_node

        while (node)
        {
            auto is_less = comp_(key, node->key());
            if (!is_less && !comp_(node->key(), key))
                break;
            
            parent = node;
            if (is_less)
            {
                node = node->get_left();
                side = Side::left;
            }
            else
            {
                node = node->get_right();
                side = Side::right;
            }
        }

        return std::make_tuple (node, parent, side);
    }

    // Finds first element that is not less than key
    const_node_ptr lower_bound (const_node_ptr node, const key_type &key) const
    {    
        const_node_ptr result = nullptr;
        while (node)
        {
            if (!comp_(node->key(), key))
            {
                result = node;
                node = node->get_left();
            }
            else
                node = node->get_right();
        }

        return result;
    }

    node_ptr lower_bound (node_ptr node, const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).lower_bound (node, key);
    }

    // Finds first element that is greater than key
    const_node_ptr upper_bound (const_node_ptr node, const key_type &key) const
    {
        const_node_ptr result = nullptr;
        while (node)
        {
            if (comp_(key, node->key()))
            {
                result = node;
                node = node->get_left();
            }
            else
                node = node->get_right();
        }

        return result;
    }

    node_ptr upper_bound (node_ptr node, const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).upper_bound (node, key);
    }

    node_ptr insert_hint_unique (const key_type &key, end_node_ptr parent, Side side)
    {
        auto new_node = new node_type{key, color_type::red};
        new_node->set_parent (parent);

        if (side == Side::left)
            parent->set_left (new_node);
        else
            new_node->parent_unsafe()->set_right (new_node);

        for (auto node = parent; node != std::addressof (end_node());
            node = static_cast<node_ptr>(node)->get_parent())
        {
            node->subtree_size_++;
        }
        end_node().subtree_size_++;

        detail::rb_insert_fixup (get_root(), new_node);

        if (new_node == leftmost_->get_left())
            leftmost_ = new_node;

        assert (search_verifier ());
        assert (red_black_verifier());
        assert (subtree_sizes_verifier ());

        return new_node;
    }

    void insert_unique (const key_type &key)
    {
        auto [node, parent, side] = find_v2 (get_root(), key);
    
        if (node == nullptr)
            insert_hint_unique (key, parent ? parent : std::addressof (end_node()), side);
    }

    bool search_verifier () const
    {
        return std::is_sorted (begin(), end(), comp_);
    }

    bool red_black_verifier () const
    {
        if (get_root() == nullptr)
            return true; // empty tree
        
        if (get_root()->get_parent() != std::addressof (end_node()))
            return false;
        
        if (!detail::is_left_child (get_root()))
            return false;

        if (get_root()->color_ != color_type::black)
            return false;

        return (detail::red_black_verifier (get_root()) != 0);
    }

    bool subtree_sizes_verifier () const noexcept
    {
        if (size() != std::distance (begin(), end()))
            return false;
        
        for (auto it = begin(), ite = end(); it != ite; ++it)
        {
            auto node = static_cast<const_node_ptr>(it.node_);
            auto nl = node->get_left();
            auto nr = node->get_right();
            auto expected_size = 1 + (nl ? nl->subtree_size_ : size_type{0}) +
                                     (nr ? nr->subtree_size_ : size_type{0});
            
            if (expected_size != node->subtree_size_)
                return false;
        }

        return true;
    }
};

template<typename Key_T, typename Compare>
bool operator== (const ARB_Tree<Key_T, Compare> &lhs, const ARB_Tree<Key_T, Compare> &rhs)
{
    return (lhs.size() == rhs.size()) &&
           (std::equal (lhs.begin(), lhs.end(), rhs.begin()));
}

template<typename Key_T, typename Compare>
auto operator<=> (const ARB_Tree<Key_T, Compare> &lhs, const ARB_Tree<Key_T, Compare> &rhs)
-> decltype (std::compare_three_way{}(*lhs.begin(), *rhs.begin()))
{
    return std::lexicographical_compare_three_way (lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
