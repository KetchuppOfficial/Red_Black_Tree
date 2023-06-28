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
void node_dump (std::ostream &os, const Node_T *node)
{
    using color_type = typename Node_T::color_type;

    os << "    node_" << node << " [shape = record, ";
    if (node->color_ == color_type::black)
        os << "color = red, style = filled, fillcolor = black, fontcolor = white";
    else
        os << "color = black, style = filled, fillcolor = red, fontcolor = black";

    os << ", label = \"key: " << node->key()
        << "| size: " << node->subtree_size_ << "\"];\n";

    if (node->get_left() == nullptr)
        os << "\tleft_nil_node_" << node << " [shape = record, "
            << "color = red, style = filled, fillcolor = black, fontcolor = white, label = \"nil\"];\n";

    if (node->get_right() == nullptr)
        os << "\tright_nil_node_" << node << " [shape = record, "
            << "color = red, style = filled, fillcolor = black, fontcolor = white, label = \"nil\"];\n";
}

template<typename Node_T>
void arrow_dump (std::ostream &os, const Node_T *node)
{
    os << "    node_" << node << " -> ";
    if (node->get_left())
        os << "node_" << node->get_left();
    else
        os << "left_nil_node_" << node;
    os << " [color = \"blue\"];\n";

    os << "    node_" << node << " -> ";
    if (node->get_right())
        os << "node_" << node->get_right();
    else
        os << "right_nil_node_" << node;
    os << " [color = \"gold\"];\n";
        
    os << "    node_" << node << " -> "
        << "node_" << node->get_parent() << " [color = \"dimgray\"];\n";
}

template<typename Node_T>
void graphic_dump (std::ostream &os, const Node_T *begin, const End_Node<Node_T> *end)
{
    using node_ptr = const Node_T *;
    using end_node_ptr = const End_Node<Node_T> *;
    
    assert (begin && end);

    os << "digraph Tree\n"
          "{\n"
          "    rankdir = TB;\n";
          "    node [shape = record];\n\n";

    os << "    node_" << end
       << " [color = black, style = filled, fillcolor = yellow, label = \"end node| "
       << "size: " << end->subtree_size_ << "\"];\n";

    for (end_node_ptr node = begin; node != end; node = successor (static_cast<node_ptr>(node)))
        node_dump (os, static_cast<node_ptr>(node));

    os << std::endl;
    for (end_node_ptr node = begin; node != end; node = successor (static_cast<node_ptr>(node)))
        arrow_dump (os, static_cast<node_ptr>(node));

    os << "    node_" << end << " -> node_" << end->get_left() << " [color = \"blue\"];\n}\n";
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_GRAPHIC_DUMP_HPP
