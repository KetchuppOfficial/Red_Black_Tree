#ifndef INCLUDE_GRAPHIC_DUMP_HPP
#define INCLUDE_GRAPHIC_DUMP_HPP

#include <fstream>

#include "rb_tree.hpp"

namespace yLab
{

namespace graphic_dump
{

template<typename Key_T>
void node_dump (std::ostream &fs, typename RB_Tree<Key_T>::iterator it)
{
    auto self = it.base();
    
    fs << "\tnode_" << self
        << " [style = filled, fillcolor = " << ((self->color_ == RB_Color::black) ? "green" : "red")
        << ", label = \"" << *it << "\"];\n";

    if (self->left_ == nullptr)
        fs << "\tleft_nil_node_" << self
            << " [style = filled, fillcolor = \"green\", label = \"nil\"];\n";

    if (self->right_ == nullptr)
        fs << "\tright_nil_node_" << self
            << " [style = filled, fillcolor = \"green\", label = \"nil\"];\n";
}

template<typename Key_T>
void arrow_dump (std::ostream &fs, typename RB_Tree<Key_T>::iterator it)
{
    auto self = it.base();
    
    fs << "\tnode_" << self << " -> ";
    if (self->left_)
        fs << "node_" << self->left_;
    else
        fs << "left_nil_node_" << self;
    fs << " [color = \"blue\"];\n";

    fs << "\tnode_" << self << " -> ";
    if (self->right_)
        fs << "node_" << self->right_;
    else
        fs << "right_nil_node_" << self;
    fs << " [color = \"gold\"];\n";
        
    fs << "\tnode_" << self << " -> "
       << "node_" << self->parent_ << " [color = \"dimgray\"];\n";
}

template<typename Key_T>
void tree_dump (std::ostream &fs, typename RB_Tree<Key_T>::iterator begin,
                                  typename RB_Tree<Key_T>::iterator end)
{
    fs << "digraph Tree\n"
          "{\n"
          "\trankdir = TB;\n"
          "\tnode [style = rounded];\n\n";

    fs << "\tnode_" << end.base()
       << " [style = filled, fillcolor = yellow, label = \"end node\"];\n";

    for (auto it = begin; it != end; ++it)
        node_dump<Key_T> (fs, it);

    fs << std::endl;
    for (auto it = begin; it != end; ++it)
        arrow_dump<Key_T> (fs, it);

    fs << "\tnode_" << end.base() << " -> node_" << end.base()->left_ << " [color = \"blue\"];\n}\n";
}

} // namespace graphic_dump

} // namespace yLab

#endif // INCLUDE_GRAPHIC_DUMP_HPP
