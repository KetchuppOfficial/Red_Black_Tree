#include <iostream>
#include <exception>
#include <set>

#include "common.hpp"

int main ()
{
    std::set<int> tree;

    while (!std::cin.eof())
    {
        char query;
        int key_;
        std::cin >> query >> key_;

        if (std::cin.fail())
            break;

        switch (query)
        {
            case end_to_end::Queries::key:
                tree.insert (key_);
                break;

            case end_to_end::Queries::kth_smallest:
            {
                auto it = tree.begin();
                std::advance (it, key_ - 1);
                std::cout << *it << " ";
                
                break;
            } 

            case end_to_end::Queries::n_less_than_given:
                std::cout << std::distance (tree.begin(), tree.lower_bound (key_)) << " ";
                break;

            default:
                throw std::runtime_error ("Unknown query");
        }
    }

    std::cout << std::endl;
    
    return 0;
}