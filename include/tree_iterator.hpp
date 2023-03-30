#ifndef INCLUDE_TREE_ITERATOR
#define INCLUDE_TREE_ITERATOR

#include <iterator>

#include "nodes.hpp"

namespace yLab
{

template <typename Key_T, typename Node_T>
requires Binary_Tree_Node<Node_T>
class tree_iterator final
{
public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Key_T;
    using reference = const Key_T &;
    using pointer = const Key_T *;

private:

    using node_ptr = Node_T *;
    
    node_ptr node_;

public:

    tree_iterator () = default;
    explicit tree_iterator (node_ptr node) noexcept : node_{node} {}

    reference operator* () const { return node_->key(); }
    pointer operator-> () const { return &node_->key(); }

    tree_iterator &operator++ ()
    {
        node_ = detail::successor (node_);
        return *this;
    }
    
    tree_iterator operator++ (int)
    {
        auto tmp = *this;
        ++tmp;
        return tmp;
    }

    tree_iterator &operator-- ()
    {
        node_ = detail::predecessor (node_);
        return *this;
    }

    tree_iterator operator-- (int)
    {
        auto tmp = *this;
        --tmp;
        return tmp;
    }

    bool operator== (const tree_iterator &rhs) const { return node_ == rhs.node_; }

    template<typename, typename, typename> friend class RB_Tree;
};

} // namespace yLab

#endif // INCLUDE_TREE_ITERATOR
