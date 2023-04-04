#include <random>
#include <exception>
#include <iostream>
#include <tuple>
#include <unordered_set>

#include "common.hpp"

using args = std::tuple<int, double, double, double>;

args cmd_line_args (int argc, char *argv[])
{
    if (argc != 5)
        throw std::runtime_error{"Program requires exactly 4 arguments"};
    
    auto n_queries = std::atoi (argv[1]);
    if (n_queries < 0)
        throw std::runtime_error{"The number of queries has to be a positive integer"};

    auto key_weight = std::atof (argv[2]);
    if (key_weight < 0)
        throw std::runtime_error{"The probability of \"insert\" query has to be a positive number"};
    
    auto kths_weight = std::atof (argv[3]); // kth smallest weight
    if (kths_weight < 0)
        throw std::runtime_error{"The probability of \"kth smallest\" query has to be a positive number"};

    auto nltg_weight = std::atof (argv[4]); // n less than given weight
    if (nltg_weight < 0)
        throw std::runtime_error{"The probability of \"number of elements less than given\" query"
                                 " has to be a positive number"};

    return std::make_tuple (n_queries, key_weight, kths_weight, nltg_weight);
}

int main (int argc, char *argv[])
{
    auto [n_queries, key_weight, kths_weight, nltg_weight] = cmd_line_args (argc, argv);

    std::random_device rd;
    std::mt19937_64 gen{rd()};
    std::discrete_distribution<int> queries = {key_weight, kths_weight, nltg_weight};
    std::uniform_int_distribution<int> key {0, 50};

    std::unordered_set<int> keys;

    for (auto query_i = 0, n_keys = 0; query_i != n_queries; ++query_i)
    {
        auto query = queries (gen);

        switch (query)
        {
            case 0:
            {
                auto key_ = key (gen);
                std::cout << end_to_end::Queries::key << " " << key_ << " ";
                keys.insert (key_);
                break;
            }   

            case 1:
            {
                if (keys.size() == 0)
                {
                    query_i--;
                    break;
                }
                    
                std::uniform_int_distribution<std::size_t> k{1, keys.size()};
                std::cout << end_to_end::Queries::kth_smallest << " " << k (gen) << " ";
                break;
            }
            
            case 2:
                std::cout << end_to_end::Queries::n_less_than_given << " " << key (gen) << " ";
                break;

            default:
                throw std::runtime_error{"Unknown query"};
        }
    }

    std::cout << std::endl;
    
    return 0;
}
