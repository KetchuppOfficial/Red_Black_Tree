/*
 * This header contains implementation of two types of nodes and some basic functions on them.
 *
 * There are 2 types of nodes: End_Node and ARB_Node. The first one has only one pointer to the
 * left child. ARB_Node is an ordinary node of a red-black tree. It also contains the number of
 * nodes in its subtree to make range-based queries work in O(log(n)) time. ARB_Node is inherited
 * from End_Node (CRTP is used).
 * 
 * End_Node is supposed to represent underlying node of end-iterator.
 * 
 * Parent of an ARB_Node can be got by means of get_parent() or parent_unsafe(). The first
 * function returns a pointer to the base class (to End_Node part of a node). The second one
 * performs downcast to a pointer to ARB_Node. It is so because in some cases we are certain 
 * that a node's parent is of type ARB_Node and in other cases we are not.
 * 
 * Functions that perform rotation (left_rotate() and right_rotate()) also recalculate sizes
 * of subtrees.
 * 
 * Successor and predecessor functions are designed the following way. Let root_ be the root 
 * of a tree and end_node_ == root->parent_, then (successor (maximum (root_)) == end_node_).
 */

#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include <type_traits>
#include <utility>
#include <cassert>

namespace yLab
{

template<typename Node_T>
class End_Node
{
    using node_ptr = Node_T *;
    using const_node_ptr = const Node_T *;

    node_ptr left_ = nullptr;

public:

    using size_type = std::size_t;

    size_type subtree_size_{1};

    End_Node () = default;

    End_Node (node_ptr left) : left_{left} {}

    End_Node (const End_Node &rhs) = delete;
    End_Node &operator= (const End_Node &rhs) = delete;

    End_Node (End_Node &&rhs) : left_{std::exchange (rhs.left_, nullptr)},
                                subtree_size_{std::exchange (rhs.subtree_size_, 1)} {}

    End_Node &operator= (End_Node &&rhs) noexcept
    {
        std::swap (left_, rhs.left_);
        std::swap (subtree_size_, rhs.subtree_size_);
        return *this;
    }

    const_node_ptr get_left () const noexcept { return left_; }
    node_ptr get_left () noexcept { return left_; }

    void set_left (node_ptr left) noexcept { left_ = left; }

    virtual ~End_Node() = default;
};

// ARB_Node - augmented red-black node
template<typename Key_T>
class ARB_Node : public End_Node<ARB_Node<Key_T>>
{
    using node_ptr = ARB_Node *;
    using const_node_ptr = const ARB_Node *;
    using base_ = End_Node<ARB_Node>;
    using end_node_ptr = base_ *;
    using const_end_node_ptr = const base_ *;

    node_ptr right_ = nullptr;
    end_node_ptr parent_ = nullptr;

    Key_T key_;

public:

    using size_type = std::size_t;
    using key_type = Key_T;

    enum class RB_Color
    {
        red,
        black
    };

    using color_type = RB_Color;

    color_type color_;

    ARB_Node (const key_type &key, color_type color) : color_{color}, key_{key} {}
    ARB_Node (key_type &&key, color_type color) : color_{color}, key_{key} {}

    ARB_Node (const ARB_Node &rhs) = delete;
    ARB_Node &operator= (const ARB_Node &rhs) = delete;

    ARB_Node (ARB_Node &&rhs)
            : base_{std::move (rhs)},
              right_{std::exchange (rhs.right_, nullptr)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              color_{std::move (rhs.color_)},
              key_{std::move (rhs.key_)} {}

    ARB_Node &operator= (ARB_Node &&rhs) noexcept (std::is_nothrow_swappable_v<key_type>)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (right_, rhs.right_);
        std::swap (parent_, rhs.parent_);
        std::swap (color_, rhs.color_);
        std::swap (key_, rhs.key_);
        
        return *this;
    }

    const_node_ptr get_right () const noexcept { return right_; }
    node_ptr get_right () noexcept { return right_; }
    void set_right (node_ptr right) noexcept { right_ = right; }

    const_end_node_ptr get_parent () const noexcept { return parent_; }
    end_node_ptr get_parent () noexcept { return parent_; }
    void set_parent (end_node_ptr parent) noexcept { parent_ = parent; }

    const_node_ptr parent_unsafe () const noexcept { return static_cast<const_node_ptr>(parent_); }
    node_ptr parent_unsafe () noexcept { return static_cast<node_ptr>(parent_); }

    const key_type &key () const { return key_; }
    static size_type size (const_node_ptr node) noexcept { return node ? node->subtree_size_ : 0; }
};

namespace detail
{

template<typename Node_Ptr>
bool is_red (Node_Ptr node) noexcept
{
    using color_type = typename std::remove_pointer_t<Node_Ptr>::color_type;

    return (node && node->color_ == color_type::red);
}

template<typename Node_Ptr>
bool is_left_child (Node_Ptr node) noexcept
{
    assert (node);
    assert (node->get_parent());
    
    return node == node->get_parent()->get_left();
}

template<typename Node_Ptr>
Node_Ptr minimum (Node_Ptr node) noexcept
{
    assert (node);
    
    while (node->get_left())
        node = node->get_left();

    return node;
}

template<typename Node_Ptr>
Node_Ptr maximum (Node_Ptr node) noexcept
{
    assert (node);
    
    while (node->get_right())
        node = node->get_right();

    return node;
}

template<typename Node_Ptr>
auto successor (Node_Ptr node) noexcept -> decltype (node->get_parent())
{
    assert (node);
    
    if (node->get_right())
        return minimum (node->get_right());

    while (!is_left_child (node))
        node = node->parent_unsafe();
    
    return node->get_parent();
}

template<typename End_Node_Ptr>
auto predecessor (End_Node_Ptr node) noexcept -> decltype (node->get_left())
{
    using node_ptr = decltype (node->get_left());

    assert (node);
    
    if (node->get_left())
        return maximum (node->get_left());

    auto node_ = static_cast<node_ptr>(node);
    while (is_left_child (node_))
        node_ = node_->parent_unsafe();

    return node_->parent_unsafe();
}

/*
 *   |               |
 *   x               y
 *  / \             / \
 * a   y    -->    x   c
 *    / \         / \
 *   b   c       a   b
 */
template<typename Node_Ptr>
void left_rotate (Node_Ptr x) noexcept
{
    using node_type = std::remove_pointer_t<Node_Ptr>;
    
    assert (x);
    assert (x->get_right());

    auto y = x->get_right();

    auto b = y->get_left();
    x->set_right (b);
    if (b)
        b->set_parent (x);

    y->set_parent (x->get_parent());
    if (is_left_child (x))
        x->get_parent()->set_left (y);
    else
        x->parent_unsafe()->set_right (y);

    y->set_left (x);
    x->set_parent (y);

    x->subtree_size_ = 1 + node_type::size (x->get_left()) + node_type::size (b);
    y->subtree_size_ = 1 + x->subtree_size_ + node_type::size (y->get_right());
}

/*
 *   |               |
 *   y               x
 *  / \             / \
 * a   x    <--    y   c
 *    / \         / \
 *   b   c       a   b
 */
template <typename Node_Ptr>
void right_rotate (Node_Ptr x) noexcept
{
    using node_type = std::remove_pointer_t<Node_Ptr>;
    
    assert (x);
    assert (x->get_left());

    auto y = x->get_left();

    auto b = y->get_right();
    x->set_left (b);
    if (b)
        b->set_parent (x);

    y->set_parent (x->get_parent());
    if (is_left_child (x))
        x->get_parent()->set_left (y);
    else
        x->parent_unsafe()->set_right (y);

    y->set_right (x);
    x->set_parent (y);

    x->subtree_size_ = 1 + node_type::size (x->get_right()) + node_type::size (b);
    y->subtree_size_ = 1 + x->subtree_size_ + node_type::size (y->get_left());
}

template<typename Node_Ptr>
Node_Ptr kth_smallest (Node_Ptr root, std::size_t k) noexcept
{
    using node_type = std::remove_pointer_t<Node_Ptr>;
    
    assert (root);
    
    if (k > root->subtree_size_)
        return nullptr;

    auto left = root->get_left();
    auto left_size = node_type::size (left);
    
    while (k != left_size + 1)
    {    
        if (0 < left_size && k <= left_size)
            root = left;
        else
        {
            root = root->get_right();
            k -= (left_size + 1);
        }

        left = root->get_left();
        left_size = node_type::size (left);
    }

    return root;
}

template<typename End_Node_Ptr>
auto n_less_than (End_Node_Ptr root, End_Node_Ptr node) noexcept ->
typename std::remove_pointer_t<End_Node_Ptr>::size_type
{
    using node_ptr = decltype (node->get_left());
    using node_type = std::remove_pointer_t<node_ptr>;

    assert (node);
    
    auto rank = node_type::size (node->get_left());

    while (node != root)
    {
        auto node_ = static_cast<node_ptr>(node);
        auto np = node_->get_parent();
        auto npl = np->get_left();
        
        if (!is_left_child (node_))
            rank += 1 + node_type::size (npl);

        node = node_->get_parent();
    }

    return rank;
}

template<typename Node_Ptr>
std::size_t red_black_verifier (Node_Ptr root) noexcept
{
    using color_type = typename std::remove_pointer_t<Node_Ptr>::color_type;
    
    if (root == nullptr)
        return 1;

    auto left = root->get_left();
    auto right = root->get_right();

    if ((left && left->get_parent() != root) ||
        (right && right->get_parent() != root))
        return 0;

    auto is_root_red = (root->color_ == color_type::red);
    if (is_root_red)
    {
        if (is_red (left) || is_red (right))
            return 0;
    }

    auto left_black_height = red_black_verifier (left);
    if (left_black_height == 0 || 
        left_black_height != red_black_verifier (right))
        return 0;

    return left_black_height + !is_root_red;
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_NODES_HPP
