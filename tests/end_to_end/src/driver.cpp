#include <iostream>
#include <iterator>
#include <exception>
#include <set>

#include "rb_tree.hpp"

int main ()
{
    enum Queries : char
    {
        key = 'k',
        kth_smallest = 'm',
        n_less_than_given = 'n'
    };

    #ifndef NAIVE
    yLab::RB_Tree<int> tree;
    #endif // NAIVE

    #ifdef NAIVE
    std::set<int> tree;
    #endif // NAIVE

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
                tree.insert (key_);
                break;

            case Queries::kth_smallest:
            {
                #ifndef NAIVE
                std::cout << *tree.kth_smallest(key_) << " ";
                #endif // NAIVE
                
                #ifdef NAIVE
                auto it = tree.begin();
                std::advance (it, key_ - 1);
                std::cout << *it << " ";
                #endif // NAIVE
                
                break;
            } 

            case Queries::n_less_than_given:
                #ifndef NAIVE
                std::cout << tree.n_less_than (key_);
                #endif // NAIVE

                #ifdef NAIVE
                std::cout << std::distance (tree.begin(), tree.lower_bound (key_)) << " ";
                #endif // NAIVE
                break;

            default:
                throw std::runtime_error ("Unknown query");
        }
    }

    std::cout << std::endl;
    
    return 0;
}
