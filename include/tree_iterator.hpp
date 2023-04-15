#ifndef INCLUDE_TREE_ITERATOR
#define INCLUDE_TREE_ITERATOR

#include <iterator>

#include "nodes.hpp"

namespace yLab
{

template <typename Node_T>
class tree_iterator final
{
public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = typename Node_T::key_type;
    using reference = const value_type &;
    using pointer = const value_type *;

private:

    using end_node_type = End_Node<Node_T>;
    using const_node_ptr = const Node_T *; 
    using const_end_node_ptr = const end_node_type *;

    const_end_node_ptr node_;

public:

    tree_iterator () = default;
    explicit tree_iterator (const_end_node_ptr node) noexcept : node_{node} {}

    reference operator* () const { return static_cast<const_node_ptr>(node_)->key(); }
    pointer operator-> () const { return &static_cast<const_node_ptr>(node_)->key(); }

    tree_iterator &operator++ () noexcept
    {
        node_ = detail::successor (static_cast<const_node_ptr>(node_));
        return *this;
    }
    
    tree_iterator operator++ (int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    tree_iterator &operator-- () noexcept
    {
        node_ = detail::predecessor (node_);
        return *this;
    }

    tree_iterator operator-- (int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator== (const tree_iterator &rhs) const noexcept { return node_ == rhs.node_; }

    template<typename key_t, typename compare> friend class ARB_Tree;
};

} // namespace yLab

#endif // INCLUDE_TREE_ITERATOR
