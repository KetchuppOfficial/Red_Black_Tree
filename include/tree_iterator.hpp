#ifndef INCLUDE_TREE_ITERATOR
#define INCLUDE_TREE_ITERATOR

#include <iterator>

#include "details.hpp"

namespace yLab
{

template <typename Key_T, typename Node_T>
class tree_iterator final
{
public:

    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Key_T;
    using reference = const Key_T&;
    using pointer = const Key_T*;

private:

    using node_ptr = Node_T *;
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

    node_ptr base () const { return node_; }
};

} // namespace yLab

#endif // INCLUDE_TREE_ITERATOR
