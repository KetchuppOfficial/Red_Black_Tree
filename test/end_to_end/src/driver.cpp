#include <iostream>
#include <exception>
#include <fstream>
#include <chrono>

#ifdef STD_SET
#include <set>
#else
#include "arb_tree.hpp"
#endif

#include "common.hpp"

int main ()
{
    #ifdef STD_SET
    std::set<int> tree;
    #else
    yLab::ARB_Tree<int> tree;
    #endif

    #ifdef STD_SET
    std::ofstream file{"ans.info"};
    #else
    std::ofstream file{"driver.info"};
    #endif
    auto start = std::chrono::high_resolution_clock::now();

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
            #ifdef STD_SET
            {
                auto it = tree.begin();
                std::advance (it, key_ - 1);
                std::cout << *it << " ";
            }
            #else
                std::cout << *tree[key_] << " ";
            #endif
                break;

            case end_to_end::Queries::n_less_than_given:
            #ifdef STD_SET
                std::cout << std::distance (tree.begin(), tree.lower_bound (key_)) << " ";
            #else
                std::cout << tree.n_less_than (key_) << " ";
            #endif
                break;

            default:
                throw std::runtime_error ("Unknown query");
        }
    }

    std::cout << std::endl;

    auto finish = std::chrono::high_resolution_clock::now();
    file << duration_cast<std::chrono::milliseconds>(finish - start).count() << std::endl;

    return 0;
}
