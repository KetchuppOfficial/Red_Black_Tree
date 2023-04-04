#include <iostream>
#include <exception>

#include "rb_tree.hpp"
#include "common.hpp"

int main ()
{
    yLab::RB_Tree<int> tree;

    while (!std::cin.eof())
    {
        char query = 0;
        int key_ = 0;

        std::cin >> query >> key_;

        if (!std::cin.good())
            break;

        switch (query)
        {
            case end_to_end::Queries::key:
                tree.insert (key_);
                break;

            case end_to_end::Queries::kth_smallest:
                std::cout << *tree.kth_smallest (key_) << " ";
                break;

            case end_to_end::Queries::n_less_than_given:
                std::cout << tree.n_less_than (key_) << " ";
                break;

            default:
                throw std::runtime_error ("Unknown query");
        }
    }

    std::cout << std::endl;
    
    return 0;
}
