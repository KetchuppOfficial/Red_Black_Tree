#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include <type_traits>
#include <utility>

namespace yLab
{

template <typename T>
class End_Node
{
    using node_ptr = T *; // may be not End_Node *

public:

    node_ptr left_;

    End_Node (node_ptr left = nullptr) : left_{left} {}

    End_Node (const End_Node &rhs) = delete;
    End_Node &operator= (const End_Node &rhs) = delete;

    End_Node (End_Node &&rhs) noexcept : left_{std::exchange (rhs.left_, nullptr)} {}

    End_Node &operator= (End_Node &&rhs) noexcept
    {
        std::swap (left_, rhs.left_);
        return *this;
    }

    virtual ~End_Node() = default;
};

template<typename T>
concept Binary_Tree_Node = 
requires { typename T::key_type; } &&
requires (T node)
{
    node.left_;   requires std::same_as<decltype (node.left_), T*>;
    node.right_;  requires std::same_as<decltype (node.right_), T*>;
    node.parent_; requires std::same_as<decltype (node.parent_), T*>;
    
    { node.key() } -> std::same_as<const typename T::key_type &>;
};

template <typename Key_T>
class RB_Node : public End_Node<RB_Node<Key_T>>
{    
    using base_ = End_Node<RB_Node>;
    using node_ptr = RB_Node *;
    using const_node_ptr = const RB_Node *;

public:

    enum class RB_Color
    {
        red,
        black
    };

    using key_type = Key_T;
    using color_type = RB_Color;

    node_ptr parent_ = nullptr;
    node_ptr right_  = nullptr;

    RB_Color color_;

private:

    key_type key_;

public:

    RB_Node (const key_type &key, color_type color) : base_{}, color_{color}, key_{key} {}

    RB_Node (RB_Node &&rhs) noexcept 
            : base_ {std::move (rhs)},
              parent_{std::exchange (rhs.parent_, nullptr)},
              right_ {std::exchange (rhs.right_,  nullptr)},
              color_ {std::move (rhs.color_)},
              key_   {std::move (rhs.key_)} {}
            
    RB_Node &operator= (RB_Node &&rhs) noexcept
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (parent_, rhs.parent_);
        std::swap (right_,  rhs.right_);
        std::swap (key_,    rhs.key_);
        std::swap (color_,  rhs.color_);

        return *this;
    }

    const key_type &key () const { return key_; }
};

template<typename Key_T>
class Advanced_RB_Node : public RB_Node<Key_T>
{
    using base_ = RB_Node<Key_T>;

public:

    using typename base_::key_type;
    using typename base_::color_type;
    using size_type = std::size_t;

private:

    size_type subtree_height_ = 0;

    Advanced_RB_Node (const key_type &key, color_type color, size_type subtree_height)
                     : RB_Node<key_type>{key, color}, subtree_height_{subtree_height} {}

    Advanced_RB_Node (Advanced_RB_Node &&rhs)
                     : base_{std::move(rhs)},
                       subtree_height_{std::exchange (rhs.subtree_height_, 0)} {}

    Advanced_RB_Node &operator= (Advanced_RB_Node &&rhs)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (subtree_height_, rhs.subtree_height_);

        return *this;
    }

    size_type subtree_height () const noexcept { return subtree_height_; }
    size_type &subtree_height () noexcept { return subtree_height_; }
};

} // namespace yLab

#endif // INCLUDE_NODES_HPP
