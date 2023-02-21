#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include <type_traits>
#include <utility>

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

} // namespace yLab

#endif // INCLUDE_NODES_HPP
