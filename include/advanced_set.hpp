#ifndef INCLUDE_ADVANCED_SET_HPP
#define INCLUDE_ADVANCED_SET_HPP

#include "rb_tree.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
using Advanced_Set = RB_Tree<Key_T, Advanced_RB_Node<Key_T>, Compare>;

}

#endif // INCLUDE_ADVANCED_SET_HPP
