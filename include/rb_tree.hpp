#ifndef INCLUDE_RB_TREE_HPP
#define INCLUDE_RB_TREE_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <cassert>
#include <fstream>
#include <initializer_list>

namespace yLab
{

template <typename Ptr_T>
requires std::is_pointer_v<Ptr_T>
class End_Node
{
    using self = End_Node<Ptr_T>;

public:

    Ptr_T left_;

    End_Node (Ptr_T left = nullptr) : left_{left} {}

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
bool is_left_child (const RB_Node<Key_T> *node) noexcept
{
    assert (node && node->parent_);
    
    return node == node->parent_->left_;
}

template<typename Key_T>
const RB_Node<Key_T> *minimum (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    while (node->left_)
        node = node->left_;

    return node;
}

template<typename Key_T>
RB_Node<Key_T> *minimum (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(minimum (static_cast<const RB_Node<Key_T> *>(node)));
}

template<typename Key_T>
const RB_Node<Key_T> *maximum (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    while (node->right_)
        node = node->right_;

    return node;
}

template<typename Key_T>
RB_Node<Key_T> *maximum (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(maximum (static_cast<const RB_Node<Key_T> *>(node)));
}

template<typename Key_T>
const RB_Node<Key_T> *successor (const RB_Node<Key_T> *node) noexcept
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
RB_Node<Key_T> *successor (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(successor (static_cast<const RB_Node<Key_T> *>(node)));
}

template<typename Key_T>
const RB_Node<Key_T> *predecessor (const RB_Node<Key_T> *node) noexcept
{
    assert (node);
    
    if (node->left_)
        return maximum (node->left_);

    auto parent = node->parent;
    while (is_left_child (node))
        node = node->parent_;

    return node->parent_;
}

template<typename Key_T>
RB_Node<Key_T> *predecessor (RB_Node<Key_T> *node) noexcept
{
    return const_cast<RB_Node<Key_T> *>(prececessor (static_cast<const RB_Node<Key_T> *>(node)));
}

template <typename Key_T>
const RB_Node<Key_T> *find (const RB_Node<Key_T> *node, const Key_T &key)
{
    while (node && key != node->key())
        node = (key < node->key()) ? node->left_ : node->right_;

    return node;
}

template <typename Key_T>
RB_Node<Key_T> *find (RB_Node<Key_T> *node, const Key_T &key)
{
    return const_cast<RB_Node<Key_T> *>(find (static_cast<const RB_Node<Key_T> *>(node), key));
}

// Finds first element that is not less than key
template <typename Key_T>
const RB_Node<Key_T> *lower_bound (const RB_Node<Key_T> *node, const Key_T &key)
{
    const RB_Node<Key_T> *result = nullptr;
    while (node)
    {
        if (key <= node->key())
        {
            result = node;
            node = node->left_;
        }
        else
            node = node->right_;
    }

    return result;
}

template <typename Key_T>
RB_Node<Key_T> *lower_bound (RB_Node<Key_T> *node, const Key_T &key)
{    
    return const_cast<RB_Node<Key_T> *>(lower_bound (static_cast<const RB_Node<Key_T> *>(node), key));
}

// Finds first element that is greater than key
template <typename Key_T>
const RB_Node<Key_T> *upper_bound (const RB_Node<Key_T> *node, const Key_T &key)
{
    const RB_Node<Key_T> *result = nullptr;
    while (node)
    {
        if (key < node->key())
        {
            result = node;
            node = node->left_;
        }
        else
            node = node->right_;
    }

    return result;
}

template <typename Key_T>
RB_Node<Key_T> *upper_bound (RB_Node<Key_T> *node, const Key_T &key)
{
    return const_cast<RB_Node<Key_T> *>(upper_bound (static_cast<const RB_Node<Key_T> *>(node), key));
}

// (parent == nullptr) ==> (key == root().key())
// (node != nullptr) ==> (parent != nullptr)
template <typename Key_T>
auto find_v2 (RB_Node<Key_T> *node, const Key_T &key)
{
    using node_ptr = RB_Node<Key_T> *;
    using result = std::pair<node_ptr, node_ptr>;
    
    node_ptr parent = nullptr;

    while (node)
    {
        if (key == node->key())
            return result{node, parent};
        
        parent = node;
        node = (key < node->key()) ? node->left_ : node = node->right_;
    }

    return result{node, parent};
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
        
        if (is_left_child (new_node->parent_))
        {
            // (new_node->parent_ != root_) ==> exitsts (new_node->parent_->parent_)
            RB_Node<Key_T> *uncle = new_node->parent_->parent_->right_;

            if (uncle && uncle->color_ == RB_Color::red)
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
            RB_Node<Key_T> *uncle = new_node->parent_->parent_->left_;

            if (uncle && uncle->color_ == RB_Color::red)
                new_node = fixup_subroutine_1 (new_node, uncle, root);
            else
            {
                if (is_left_child (new_node->parent_))
                {
                    new_node = new_node->parent_;
                    details::right_rotate (new_node);
                }

                left_rotate (fixup_subroutine_2 (new_node));
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
public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

private:

    using node_ptr = RB_Node<value_type> *;
    using const_node_ptr = const RB_Node<value_type> *;
    using self = tree_iterator;
    
    node_ptr node_;

public:

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

    const_node_ptr base () const { return node_; }
};

template <typename Key_T>
class const_tree_iterator final
{
public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = const Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

private:

    using node_ptr = const RB_Node<Key_T> *;
    using self = const_tree_iterator;
    
    node_ptr node_;

public:

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

    node_ptr base () const { return node_; }
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
public:

    using key_type = Key_T;
    using value_type = key_type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using node_type = RB_Node<key_type>;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = iterators::tree_iterator<key_type>;
    using const_iterator = iterators::const_tree_iterator<key_type>;

private:

    using self = RB_Tree<key_type>;
    using node_ptr = node_type *;
    using const_node_ptr = const node_type *;
    using end_node_type = End_Node<node_ptr>;
    using end_node_ptr = end_node_type *;

    end_node_ptr end_node_ = new end_node_type{};

    node_ptr leftmost_  = end_node();
    node_ptr rightmost_ = nullptr;

    std::size_t size_ = 0;

public:

    RB_Tree () = default;

    RB_Tree (const self &rhs) : size_{rhs.size_}
    {
        if (rhs.root())
        {
            auto rhs_node = const_cast<node_ptr>(rhs.root());
            root() = new node_type{rhs_node->key(), rhs_node->color_};
            root()->parent_ = end_node();

            node_ptr node = root();
            while (rhs_node != rhs.end_node())
            {
                if (rhs_node->left_ && node->left_ == nullptr)
                {
                    rhs_node = rhs_node->left_;
                    node->left_ = new node_type{rhs_node->key(), rhs_node->color_};
                    node->left_->parent_ = node;
                    node = node->left_;

                    if (rhs_node == rhs.leftmost_)
                        leftmost_ = node;
                }
                else if (rhs_node->right_ && node->right_ == nullptr)
                {
                    rhs_node = rhs_node->right_;
                    node->right_ = new node_type{rhs_node->key(), rhs_node->color_};
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

    self &operator= (const self &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    RB_Tree (self &&rhs) noexcept
            : end_node_{std::move (rhs.end_node_)},
              leftmost_{std::exchange (rhs.leftmost_, rhs.end_node())},
              rightmost_{std::exchange (rhs.rightmost_, nullptr)},
              size_{std::exchange (rhs.size_, 0)} {}

    self &operator= (self &&rhs) noexcept
    {
        std::swap (end_node_, rhs.end_node_);
        std::swap (leftmost_, rhs.leftmost_);
        std::swap (rightmost_, rhs.rightmost_);
        std::swap (size_, rhs.size_);

        return *this;
    }

    ~RB_Tree ()
    {
        for (node_ptr node = root(), save{}; node != nullptr; node = save)
        {
            if (node->left_ == nullptr)
            {
                save = node->right_;
                delete node;
            }
            else
            {
                save = node->left_;
                node->left_ = save->right_;
                save->right_ = node;
            }
        }

        delete end_node_;
    }

    // Capacity

    auto size () const { return size_; }
    bool empty () const { return size_ == 0; }

    // Iterators

    auto begin () { return iterator{leftmost_}; }
    auto begin () const { return const_iterator{leftmost_}; }
    auto cbegin () const { return const_iterator{leftmost_}; }

    auto end () { return iterator{end_node()}; }
    auto end () const { return const_iterator{end_node()}; }
    auto cend () const { return const_iterator{end_node()}; }

    // Modifiers

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

    node_ptr &root () noexcept { return end_node_->left_; }
    const_node_ptr root () const noexcept { return end_node_->left_; }

    node_ptr end_node () noexcept { return static_cast<node_ptr>(end_node_); }
    const_node_ptr end_node () const noexcept { return static_cast<node_ptr>(end_node_); }

    node_ptr insert_root (const key_type &key)
    {
        auto new_node = new node_type{key, RB_Color::black};

        root() = new_node;
        root()->parent_ = end_node();

        leftmost_ = rightmost_ = new_node;
        size_++;

        return new_node;
    }

    node_ptr insert_hint_unique (node_ptr parent, const key_type &key)
    {
        auto new_node = new node_type{key, RB_Color::red};
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

        size_++;

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

namespace graphic_dump
{

template<typename Key_T>
void node_dump (std::ostream &fs, typename RB_Tree<Key_T>::iterator it)
{
    auto self = it.base();
    
    fs << "\tnode_" << self
        << " [style = filled, fillcolor = " << ((self->color_ == RB_Color::black) ? "green" : "red")
        << ", label = \"" << *it << "\"];\n";

    if (self->left_ == nullptr)
        fs << "\tleft_nil_node_" << self
            << " [style = filled, fillcolor = \"green\", label = \"nil\"];\n";

    if (self->right_ == nullptr)
        fs << "\tright_nil_node_" << self
            << " [style = filled, fillcolor = \"green\", label = \"nil\"];\n";
}

template<typename Key_T>
void arrow_dump (std::ostream &fs, typename RB_Tree<Key_T>::iterator it)
{
    auto self = it.base();
    
    fs << "\tnode_" << self << " -> ";
    if (self->left_)
        fs << "node_" << self->left_;
    else
        fs << "left_nil_node_" << self;
    fs << " [color = \"blue\"];\n";

    fs << "\tnode_" << self << " -> ";
    if (self->right_)
        fs << "node_" << self->right_;
    else
        fs << "right_nil_node_" << self;
    fs << " [color = \"gold\"];\n";
        
    fs << "\tnode_" << self << " -> "
       << "node_" << self->parent_ << " [color = \"dimgray\"];\n";
}

template<typename Key_T>
void tree_dump (std::ostream &fs, typename RB_Tree<Key_T>::iterator begin,
                                  typename RB_Tree<Key_T>::iterator end)
{
    fs << "digraph Tree\n"
          "{\n"
          "\trankdir = TB;\n"
          "\tnode [style = rounded];\n\n";

    fs << "\tnode_" << end.base()
       << " [style = filled, fillcolor = yellow, label = \"end node\"];\n";

    for (auto it = begin; it != end; ++it)
        node_dump<Key_T> (fs, it);

    fs << std::endl;
    for (auto it = begin; it != end; ++it)
        arrow_dump<Key_T> (fs, it);

    fs << "\tnode_" << end.base() << " -> node_" << end.base()->left_ << " [color = \"blue\"];\n}\n";
}

} // namespace graphic_dump

} // namespace yLab

#endif // INCLUDE_RB_TREE_HPP
