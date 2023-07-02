/*
 * This header contains implementation of augmented red-black tree (ARB_Tree)
 *
 * ARB_Tree is designed the following way. Its root's parent is an End_Node
 * which left child is the root. This implies that every ARB_Node in a tree
 * has a parent: another ARB_Node or the End_Node.
 * 
 * Defining DEBUG macro makes it possible to call graphic_dump() method that
 * is designed for dumping a tree by means of graphviz for debugging or just
 * for fun.
 */

#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <initializer_list>
#include <functional>
#include <type_traits>
#include <cassert>
#include <compare>
#include <tuple>

#include "nodes.hpp"
#include "tree_iterator.hpp"

#ifdef DEBUG
#include <iostream>
#include "graphic_dump.hpp"
#endif // DEBUG

namespace yLab
{

namespace detail
{

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INSERTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
 * Properties that:
 * 1. parent->parent_ exists
 * 2. parent->parent_ isn't end_node
 * are guaranteed by the algorithm (look rb_insert_fixup)
 */
template<typename Node_T>
Node_T *recolor_parent_grandparent_uncle (Node_T *parent, Node_T *uncle, 
                                          const Node_T *root) noexcept
{
    using color_type = typename Node_T::color_type;

    assert (parent);
    assert (uncle);
    assert (parent->parent_unsafe());
    
    parent->color_ = color_type::black;

    parent = parent->parent_unsafe();
    if (parent != root)
        parent->color_ = color_type::red;

    uncle->color_ = color_type::black;

    return parent;
};

template<typename Node_T>
Node_T *recolor_parent_grandparent (Node_T *parent) noexcept
{
    using color_type = typename Node_T::color_type;

    assert (parent);
    assert (parent->parent_unsafe());
    
    parent->color_ = color_type::black;

    parent = parent->parent_unsafe();
    parent->color_ = color_type::red;

    return parent;
};

template <typename Node_T>
void rb_insert_fixup (const Node_T *root, Node_T *new_node) noexcept
{       
    using color_type = typename Node_T::color_type;
    
    assert (new_node);
    
    // Checks if "The root is black" property is violated
    if (new_node == root)
    {
        new_node->color_ = color_type::black;
        return;
    }

    // Further: (new_node != root) ==> (root->color_ == color_type::black)

    auto parent = new_node->parent_unsafe();

    // Checks if "If a node is red, then both its children are black" property is violated
    while (new_node != root && parent->color_ == color_type::red)
    {
        /* 
         * Some notes:
         * (1). First condition is important only for iterations 2, 3, ... but not for 1
         * (2). (new_node != root) ==> (parent != end_node)
         * (3). (parent->color_ == color_type::red) ==> (parent != root)
         */
        
        if (is_left_child (parent))
        {
            // (3) ==> exists (parent->parent_) != end_node
            // Further: we will refer to parent->parent_ as "grandparent"
            auto uncle = parent->parent_unsafe()->get_right();

            if (is_red (uncle))
            {
                /* (uncle->color_ == color_type::red) ==> grandparent->color_ == color_type::black
                 * ==> grandparent may be root */
                
                new_node = recolor_parent_grandparent_uncle (parent, uncle, root);
            }
            else
            {
                if (!is_left_child (new_node))
                {
                    left_rotate (parent);
                    parent = new_node;
                }

                /* If grandparent is root and colored red inside recolor_parent_grandparent,
                 * rotation will put parent (that is black) in place of root */
                right_rotate (recolor_parent_grandparent (parent));
                break;
            }
        }
        else
        {
            auto uncle = parent->get_parent()->get_left();

            if (is_red (uncle))
                new_node = recolor_parent_grandparent_uncle (parent, uncle, root);
            else
            {
                if (is_left_child (new_node))
                {
                    right_rotate (parent);
                    parent = new_node;
                }

                left_rotate (recolor_parent_grandparent (parent));
                break;
            }
        }

        parent = new_node->parent_unsafe();
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ERASURE FIXUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum class Side { left, right };

template<typename Node_T>
void y_has_red_sibling (Node_T *&root, Node_T *&sibling_of_y, Side side)
{
    using color_type = typename Node_T::color_type;

    assert (sibling_of_y);
    
    auto parent = sibling_of_y->parent_unsafe();

    sibling_of_y->color_ = color_type::black;
    parent->color_ = color_type::red;

    Node_T *nephew_of_y, *new_sibling_of_y;
    if (side == Side::left)
    {
        left_rotate (parent);
        nephew_of_y = sibling_of_y->get_left();
        new_sibling_of_y = nephew_of_y->get_right();
    }
    else
    {
        right_rotate (parent);
        nephew_of_y = sibling_of_y->get_right();
        new_sibling_of_y = nephew_of_y->get_left();
    }

    if (root == nephew_of_y)
        root = sibling_of_y;

    sibling_of_y = new_sibling_of_y;
}

template<typename Node_T>
void y_has_red_nephew (Node_T *sibling_of_y, Node_T *nephew_1, Node_T *nephew_2) noexcept
{
    using color_type = typename Node_T::color_type;

    assert (sibling_of_y);
    assert (nephew_2);
    assert (nephew_1 != nephew_2);
    assert (nephew_1 == sibling_of_y->get_left() || nephew_1 == sibling_of_y->get_right());
    assert (nephew_2 == sibling_of_y->get_left() || nephew_2 == sibling_of_y->get_right());
    
    bool children_in_reverse = false;
    if (is_left_child (nephew_2))
        children_in_reverse = true;
    
    if (!is_red (nephew_2))
    {
        nephew_1->color_ = color_type::black;
        sibling_of_y->color_ = color_type::red;

        if (children_in_reverse)
            left_rotate (sibling_of_y);
        else
            right_rotate (sibling_of_y);
        
        sibling_of_y = sibling_of_y->parent_unsafe();
    }

    auto parent = sibling_of_y->parent_unsafe();
    sibling_of_y->color_ = parent->color_;
    parent->color_ = color_type::black;
    nephew_2->color_ = color_type::black;

    if (children_in_reverse)
        right_rotate (parent);
    else
        left_rotate (parent);
}

template<typename Node_T>
bool y_has_no_red_nephew (const Node_T *root, Node_T *&child_of_y, Node_T *&sibling_of_y)
{
    using color_type = typename Node_T::color_type;

    assert (root);
    assert (sibling_of_y);
        
    sibling_of_y->color_ = color_type::red;
    child_of_y = sibling_of_y->parent_unsafe();

    auto to_continue = true;
    if (child_of_y == root || child_of_y->color_ == color_type::red)
    {
        child_of_y->color_ = color_type::black;
        to_continue = false;
    }

    if (is_left_child (child_of_y))
        sibling_of_y = child_of_y->parent_unsafe()->get_right();
    else
        sibling_of_y = child_of_y->get_parent()->get_left();

    return to_continue;
}

template<typename Node_T>
void rb_erase_fixup (Node_T *root, Node_T *child_of_y, Node_T *sibling_of_y)
{
    using color_type = typename Node_T::color_type;

    assert (root);
    assert (sibling_of_y);

    if (child_of_y)
    {
        child_of_y->color_ = color_type::black;
        return;
    }

    while (true)
    {
        if (is_left_child (sibling_of_y))
        {
            if (sibling_of_y->color_ == color_type::red)
                y_has_red_sibling (root, sibling_of_y, Side::right);

            auto y_l_nephew = sibling_of_y->get_left();
            auto y_r_nephew = sibling_of_y->get_right();
            if (is_red (y_l_nephew) || is_red (y_r_nephew))
                y_has_red_nephew (sibling_of_y, y_r_nephew, y_l_nephew);
            else
            {
                auto to_continue = y_has_no_red_nephew (root, child_of_y, sibling_of_y);
                if (to_continue)
                    continue;
            }
        }
        else
        {
            if (sibling_of_y->color_ == color_type::red)
                y_has_red_sibling (root, sibling_of_y, Side::left);

            auto y_l_nephew = sibling_of_y->get_left();
            auto y_r_nephew = sibling_of_y->get_right();
            if (is_red (y_l_nephew) || is_red (y_r_nephew))
                y_has_red_nephew (sibling_of_y, y_l_nephew, y_r_nephew);
            else
            {
                auto to_continue = y_has_no_red_nephew (root, child_of_y, sibling_of_y);
                if (to_continue)
                    continue;
            }
        }

        break;
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ERASURE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template<typename Node_T>
std::pair<Node_T *, Node_T *> get_y_and_its_child (Node_T *z) noexcept
{
    assert (z);
    
    // If a node has a right child, that node's successor exists
    Node_T *y;
    if (z->get_left() && z->get_right())
        y = static_cast<Node_T *>(successor (z));
    else
        y = z;

    // (child_of_y == nullptr) <==> (y->left_ == nullptr) && (y->right_ == nullptr)
    auto child_of_y = y->get_left() ? y->get_left() : y->get_right();

    return std::pair{y, child_of_y};
}

template<typename Node_T>
auto child_of_y_substitutes_y (Node_T *&root, Node_T *y, Node_T *child_of_y) noexcept
-> std::pair<Node_T *, typename Node_T::size_type>
{
    assert (y);
    assert (child_of_y == y->get_left() || child_of_y == y->get_right());
    
    Node_T *sibling_of_y = nullptr;
    if (is_left_child (y))
    {
        /* 
         * STATEMENT (*):
         * Let y == successor (z), then:
         * (1). z->left_ != nullptr
         * (2). is_left_child (y) ==> (y != z->right_) ==> (y->parent_ != z)
         * So next line doesn't change z->left_ and still isn't equal to nullptr
         * 
         * P.s: (2) is also obviously true if y == z
         */
        y->get_parent()->set_left (child_of_y);
        
        if (y != root)
        {
            // (y != root) ==> exists y->parent_ != end_node
            sibling_of_y = y->parent_unsafe()->get_right();
        }
        else
            root = child_of_y;
    }
    else
    {
        // (y == y->parent_->right_) ==> exists y->parent != end_node
        auto yp = y->parent_unsafe();

        // STATEMENT (**): if (yp == z), then next line may set z->right_ to nullptr
        yp->set_right (child_of_y);
        sibling_of_y = yp->get_left();
    }

    auto decrement = y->subtree_size_;
    if (child_of_y)
    {
        child_of_y->set_parent (y->get_parent());
        decrement -= child_of_y->subtree_size_;
    }

    y->get_parent()->subtree_size_ -= decrement;

    return std::pair{sibling_of_y, decrement};
}

// Order of y and z is significant. This function mustn't be called with arbitrary pointers
template<typename Node_T>
void decrement_subtee_hights_from_y_to_z (Node_T *y, Node_T *z,
                                          typename Node_T::size_type decrement) noexcept
{
    assert (y);

    // y->parent_->subtree_hight_ has already been changed in child_of_y_substitutes_y()
    
    if (auto yp = y->parent_unsafe(); yp != z)
    {
        for (auto ypp = yp->parent_unsafe(); ypp != z; ypp = ypp->parent_unsafe())
            ypp->subtree_size_ -= decrement;
    }
}

// Order of y and z is significant. This function mustn't be called with arbitrary pointers
template<typename Node_T>
void y_substitutes_z (Node_T *y, Node_T *z, typename Node_T::size_type z_size) noexcept
{
    assert (y);
    assert (z);
    
    auto zl = z->get_left();
    auto zr = z->get_right();

    y->set_left (zl);
    y->set_right (zr);

    // We are sure that zl != nullptr because of STATEMENT (*)
    zl->set_parent (y);
    y->subtree_size_ = zl->subtree_size_ + 1;

    if (zr) // We are NOT sure that zr != nullptr because of STATEMENT (**)
    {
        zr->set_parent (y);
        y->subtree_size_ += zr->subtree_size_;
    }

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
}

template<typename Node_T, typename End_Node_T>
void decrement_subtree_hights_upper_z (Node_T *z, End_Node_T *end_node) noexcept
{
    assert (z);
    assert (end_node);
    assert (z->get_parent() != end_node);

    /* 
     * if (y == z), then z->parent_->subtree_hight_ has already been changed
     * in child_of_y_substitutes_y()
     *
     * if (y != z), then z->parent_->subtree_hight_ has already been changed
     * in y_substitutes_z()
     */
    
    auto zpp = z->parent_unsafe()->get_parent();
    while (zpp != end_node)
    {
        zpp->subtree_size_--;
        zpp = static_cast<Node_T *>(zpp)->get_parent();
    }
    end_node->subtree_size_--;
}

template<typename Node_T>
void erase (Node_T *root, Node_T *z)
{    
    using color_type = typename Node_T::color_type;
    using size_type = typename Node_T::size_type;
    
    assert (root);
    assert (z);
    
    auto [y, child_of_y] = get_y_and_its_child (z);

    // child_of_y_substitutes_y() may change root, so we save a pointer to end_node
    // child_of_y_substitutes_y() may change z->subtree_size_, so we save it
    auto end_node = root->get_parent();
    auto z_size = z->subtree_size_;

    auto [sibling_of_y, decrement] = child_of_y_substitutes_y (root, y, child_of_y);

    // y_substitutes_z() changes y->color_, so we save it
    auto y_original_color = y->color_;

    if (y != z)
    {
        decrement_subtee_hights_from_y_to_z (y, z, decrement);
        y_substitutes_z (y, z, z_size);
        
        if (z == root)
            root = y;
    }

    if (z->get_parent() != end_node)
        decrement_subtree_hights_upper_z (z, end_node);

    if (y_original_color == color_type::black && root)
        rb_erase_fixup (root, child_of_y, sibling_of_y);
}

} // namespace detail

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

        Root_Wrapper (Root_Wrapper &&rhs) noexcept (std::is_nothrow_move_constructible_v<end_node_type>)
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
    const_end_node_ptr leftmost_ = std::addressof (end_node());
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

    ARB_Tree (ARB_Tree &&rhs)
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
    iterator end () noexcept { return iterator{std::addressof (end_node())}; }
    const_iterator end () const noexcept { return const_iterator{std::addressof (end_node())}; }

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
            auto actual_parent = parent ? parent : std::addressof (end_node());
            auto new_node = insert_hint_unique (key, actual_parent, side);

            return std::pair{iterator{new_node}, true};
        }
        else
            return std::pair{iterator{node}, false};
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
        return node ? const_iterator{node} : end();
    }

    iterator find (const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).find (key);
    }

    const_iterator lower_bound (const key_type &key) const
    {
        auto node = lower_bound (get_root(), key);
        return node ? const_iterator{node} : end();
    }

    iterator lower_bound (const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).lower_bound (key);
    }

    const_iterator upper_bound (const key_type &key) const
    {
        auto node = upper_bound (get_root(), key);
        return node ? const_iterator{node} : end();
    }

    iterator upper_bound (const key_type &key)
    {
        return static_cast<const ARB_Tree &>(*this).upper_bound (key);
    }

    bool contains (const key_type &key) const { return find (key) != end(); }

    // k-th smallest element
    const_iterator operator[] (size_type k) const
    {
        if (empty() || k == 0)
            return end();
        
        auto node = detail::kth_smallest (get_root(), k);
        return node ? const_iterator{node} : end();
    }

    iterator operator[] (size_type k)
    {
        return static_cast<const ARB_Tree &>(*this).operator[] (k);
    }

    size_type n_less_than (const key_type &key) const
    {
        if (empty())
            return 0;

        auto it = lower_bound (key);
        if (it == end())
            return size();
        else
            return detail::n_less_than<const end_node_type> (get_root(), it.node_);
    }

    #ifdef DEBUG
    
    // I see how this violates SRP but I don't know any better implementation
    void graphic_dump (std::ostream &os = std::cout) const
    {
        if (empty())
            return;

        detail::graphic_dump (os, static_cast<const_node_ptr>(leftmost_), std::addressof (end_node()));
    }

    #endif // DEBUG

private:

    end_node_type &end_node () { return root_.end_node_; }
    const end_node_type &end_node () const { return root_.end_node_; }

    void set_root (node_ptr new_root) noexcept { return end_node().set_left (new_root); }
    const_node_ptr get_root () const noexcept { return end_node().get_left(); }
    node_ptr get_root () noexcept { return end_node().get_left(); }

    const_node_ptr find (const_node_ptr node, const key_type &key) const
    {
        while (node && (comp_(key, node->key()) || comp_(node->key(), key)))
            node = comp_(key, node->key()) ? node->get_left() : node->get_right();

        return node;
    }

    using Side = detail::Side;

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

        assert (search_verifier());
        assert (red_black_verifier());
        assert (subtree_sizes_verifier());

        return new_node;
    }

    void insert_unique (const key_type &key)
    {
        auto [node, parent, side] = find_v2 (get_root(), key);
    
        if (node == nullptr)
        {
            auto actual_parent = parent ? parent : std::addressof (end_node());
            insert_hint_unique (key, actual_parent, side);
        }   
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

    bool subtree_sizes_verifier () const
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
