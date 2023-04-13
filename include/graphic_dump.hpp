#ifndef INCLUDE_GRAPHIC_DUMP_HPP
#define INCLUDE_GRAPHIC_DUMP_HPP

#include <ostream>
#include <cassert>

#include "nodes.hpp"

namespace yLab
{

namespace detail
{

template<typename Node_T>
void graphic_dump (std::ostream &os, const Node_T *begin, const End_Node<Node_T> *end)
{
    using node_ptr = const Node_T *;
    using end_node_ptr = const End_Node<Node_T> *;
    
    assert (begin && end);
    
    auto node_dump = [&os](node_ptr node)
    {
        using color_type = typename Node_T::color_type;
        
        os << "    node_" << node
           << " [style = filled, fillcolor = "
           << ((node->color_ == color_type::black) ? "black, fontcolor = white"
                                                   : "red, fontcolor = black")
           << ", label = \"key: " << node->key()
           << "| size: " << node->subtree_size_ << "\"];\n";

        if (node->left_ == nullptr)
            os << "\tleft_nil_node_" << node
               << " [style = filled, fillcolor = black, fontcolor = white, label = \"nil\"];\n";

        if (node->right_ == nullptr)
            os << "\tright_nil_node_" << node
               << " [style = filled, fillcolor = black, fontcolor = white, label = \"nil\"];\n";
    };

    auto arrow_dump = [&os](node_ptr node)
    {
        os << "    node_" << node << " -> ";
        if (node->left_)
            os << "node_" << node->left_;
        else
            os << "left_nil_node_" << node;
        os << " [color = \"blue\"];\n";

        os << "    node_" << node << " -> ";
        if (node->right_)
            os << "node_" << node->right_;
        else
            os << "right_nil_node_" << node;
        os << " [color = \"gold\"];\n";
            
        os << "    node_" << node << " -> "
           << "node_" << node->parent_ << " [color = \"dimgray\"];\n";
    };

    os << "digraph Tree\n"
          "{\n"
          "    rankdir = TB;\n"
          "    node [shape = record];\n\n";

    os << "    node_" << end
       << " [style = filled, fillcolor = yellow, label = \"end node| "
       << "size: " << end->subtree_size_ << "\"];\n";

    for (end_node_ptr node = begin; node != end; node = successor (static_cast<node_ptr>(node)))
        node_dump (static_cast<node_ptr>(node));

    os << std::endl;
    for (end_node_ptr node = begin; node != end; node = successor (static_cast<node_ptr>(node)))
        arrow_dump (static_cast<node_ptr>(node));

    os << "    node_" << end << " -> node_" << end->left_ << " [color = \"blue\"];\n}\n";
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_GRAPHIC_DUMP_HPP
