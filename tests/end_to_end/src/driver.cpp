#include <iostream>
#include <exception>
#include <fstream>
#include <chrono>

#include "rb_tree.hpp"
#include "common.hpp"

int main ()
{
    yLab::RB_Tree<int> tree;

    std::ofstream file{"driver.info"};
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

    auto finish = std::chrono::high_resolution_clock::now();
    file << duration_cast<std::chrono::milliseconds>(finish - start).count() << std::endl;
    
    return 0;
}
