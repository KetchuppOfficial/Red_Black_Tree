#include <iostream>
#include <iterator>
#include <exception>

#include "rb_tree.hpp"

int main ()
{
    enum Queries : char
    {
        key = 'k',
        kth_smallest = 'm',
        n_less_than_given = 'n'
    };

    yLab::RB_Tree<int> set;

    while (!std::cin.eof())
    {
        char query;
        int key_;
        std::cin >> query >> key_;

        if (std::cin.fail())
            break;

        switch (query)
        {
            case Queries::key:
                set.insert (key_);
                break;

            case Queries::kth_smallest:
            {
                auto it = set.begin();
                std::advance (it, key_ - 1);
                std::cout << *it << " ";
                break;
            } 

            case Queries::n_less_than_given:
                std::cout << std::distance (set.begin(), set.lower_bound (key_)) << " ";
                break;

            default:
                throw std::runtime_error ("Unknown query");
        }
    }

    std::cout << std::endl;
    
    return 0;
}
