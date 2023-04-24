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
    using const_node_ptr = const Node_T *;

    node_ptr left_ = nullptr;

public:

    using size_type = std::size_t;

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

    const_node_ptr get_left () const { return left_; }
    node_ptr get_left () { return left_; }

    void set_left (node_ptr left) { left_ = left; }

    virtual ~End_Node() = default;
};

// ARB_Node - advanced red-black node
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

    const_node_ptr get_right () const { return right_; }
    node_ptr get_right () { return right_; }
    void set_right (node_ptr right) { right_ = right; }

    const_end_node_ptr get_parent () const { return parent_; }
    end_node_ptr get_parent () { return parent_; }
    void set_parent (end_node_ptr parent) { parent_ = parent; }

    const_node_ptr parent_unsafe () const { return static_cast<const_node_ptr>(parent_); }
    node_ptr parent_unsafe () { return static_cast<node_ptr>(parent_); }

    const key_type &key () const { return key_; }
};

namespace detail
{

template<typename Node_T>
bool is_left_child (const Node_T *node) noexcept
{
    assert (node && node->get_parent());
    
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

template<typename Node_T>
void left_rotate (Node_T *x) noexcept
{
    assert (x && x->get_right());

    auto y = x->get_right();

    auto yl = y->get_left();
    x->set_right (yl);
    if (yl)
        yl->set_parent (x);

    y->set_parent (x->get_parent());
    if (is_left_child (x))
        x->get_parent()->set_left (y);
    else
        x->parent_unsafe()->set_right (y);

    y->set_left (x);
    x->set_parent (y);
}

template<typename Key_T>
void left_rotate_plus (ARB_Node<Key_T> *x) noexcept
{
    using size_type = typename ARB_Node<Key_T>::size_type;
    
    assert (x && x->get_right());
    
    auto y = x->get_right();

    auto a_size = x->subtree_size_ - y->subtree_size_ - 1;
    auto b_size = (y->get_left()) ? y->get_left()->subtree_size_ : size_type{0};
    auto c_size = (y->get_right()) ? y->get_right()->subtree_size_ : size_type{0};

    left_rotate (x);

    auto x_size = a_size + b_size + 1;
    x->subtree_size_ = x_size;
    y->subtree_size_ = x_size + c_size + 1;
}

template <typename Node_T>
void right_rotate (Node_T *x) noexcept
{
    assert (x && x->get_left());

    auto y = x->get_left();

    auto yr = y->get_right();
    x->set_left (yr);
    if (yr)
        yr->set_parent (x);

    y->set_parent (x->get_parent());
    if (is_left_child (x))
        x->get_parent()->set_left (y);
    else
        x->parent_unsafe()->set_right (y);

    y->set_right (x);
    x->set_parent (y);
}

template<typename Key_T>
void right_rotate_plus (ARB_Node<Key_T> *x) noexcept
{
    using size_type = typename ARB_Node<Key_T>::size_type;
    
    assert (x && x->get_left());
    
    auto y = x->get_left();
    auto yl = y->get_left();
    auto yr = y->get_right();

    auto c_size = x->subtree_size_ - y->subtree_size_ - 1;
    auto a_size = yl ? yl->subtree_size_ : size_type{0};
    auto b_size = yr ? yr->subtree_size_ : size_type{0};

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

    auto left = root->get_left();
    auto left_size = (left) ? left->subtree_size_ : size_type{0};
    
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
        left_size = (left) ? left->subtree_size_ : size_type{0};
    }

    return root;
}

template<typename End_Node_Ptr>
auto n_less_than (End_Node_Ptr root, End_Node_Ptr node) noexcept ->
typename std::remove_pointer_t<End_Node_Ptr>::size_type
{
    using size_type = std::remove_pointer_t<End_Node_Ptr>::size_type;
    using node_type = decltype (node->get_left());
    
    auto left = node->get_left();
    auto rank = left ? left->subtree_size_ : size_type{0};

    while (node != root)
    {
        auto node_ = static_cast<node_type>(node);
        auto np = node_->get_parent();
        auto npl = np->get_left();
        
        if (!is_left_child (node_))
            rank += npl ? npl->subtree_size_ + 1 : 1;

        node = node_->get_parent();
    }

    return rank;
}

template<typename Node_T>
std::size_t red_black_verifier (const Node_T *root) noexcept
{
    using color_type = typename Node_T::color_type;
    
    if (root == nullptr)
        return 1;

    auto left = root->get_left();
    auto right = root->get_right();

    if ((left && left->get_parent() != root) &&
        (right && right->get_parent() != root))
        return 0;

    if (root->color_ == color_type::red)
    {
        if ((left  && left->color_  == color_type::red) ||
            (right && right->color_ == color_type::red))
            return 0;
    }

    auto left_black_height = red_black_verifier (left);
    if (left_black_height == 0)
        return 0;
    if (left_black_height != red_black_verifier (right))
        return 0;

    return left_black_height + ((root->color_ == color_type::black) ? 1 : 0);
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_NODES_HPP
