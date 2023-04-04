#!/bin/bash

# argv[1]: the number of queries
# argv[2]: weight of "insert" query
# argv[3]: weight of "kth smallest" query
# argv[4]: weight of "n less than" query

green="\033[1;32m"
red="\033[1;31m"
default="\033[0m"

top_dir="../../"
build_dir="${top_dir}build/"
data="data/"
bin_dir="bin/"

test_generator="generator"
test_driver="driver"
ans_generator="ans_generator"

function build_from_sources
{
    local basic_options="-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-11"
    
    cmake ${top_dir} -B ${build_dir} ${basic_options}

    echo -en "\n"
    echo "Building test generator..."
    cmake --build ${build_dir} --target ${test_generator}
    echo -en "\n"

    echo "Building test driver..."
    cmake --build ${build_dir} --target ${test_driver}
    echo -en "\n"

    echo "Building generator of answers..."
    echo -en "\n"
    cmake --build ${build_dir} --target ${ans_generator}
    echo -en "\n"

    echo "Installing..."
    cmake --install ${build_dir}
    echo -en "\n"
}

function generate_test
{
    local n_queries=$1
    local insert_weight=$2
    local kths_weight=$3
    local nlt_weight=$4

    mkdir -p ${data}

    echo "Generating test..."
    ${bin_dir}${test_generator} ${n_queries} ${insert_weight} ${kths_weight} ${nlt_weight} > "${data}${n_queries}.test"
    echo -en "\n"
}

function generate_answer
{
    local n_queries=$1

    echo "Generating answer..."
    ${bin_dir}${ans_generator} < "${data}${n_queries}.test" > "${data}${n_queries}.ans"
    echo -en "\n"
}

function run_test
{
    local n_queries=$1

    echo "Running test..."
    ${bin_dir}${test_driver} < "${data}${n_queries}.test" > "${data}${n_queries}.res"
    echo -en "\n"

    echo -en "Result: "
    if diff -Z "${data}${n_queries}.ans" "${data}${n_queries}.res" > /dev/null
    then
        echo -e "${green}passed${default}"
    else
        echo -e "${red}failed${default}"
    fi
}

if [ $# -ne 4 ]
then
    echo "Testing script requires exactly 4 arguments"
else
    n_queries=$1

    if [ $n_queries -le 0 ]
    then
        echo "The number of queries has to be a positive integer number"
    else
        insert_weight=$2

        if [ $insert_weight -le 0 ]
        then
            echo "The probability of \"insert\" query has to be a positive number"
        else
            kths_weight=$3

            if [ $kths_weight -le 0 ]
            then
                echo "The probability of \"kth smallest\" query has to be a positive number"
            else
                nlt_weight=$4

                if [ $nlt_weight -le 0 ]
                then
                    echo "The probability of \"n less than\" query has to be a positive number"
                else
                    build_from_sources
                    generate_test $n_queries $insert_weight $kths_weight $nlt_weight
                    generate_answer $n_queries
                    run_test $n_queries
                fi
            fi
        fi
    fi
fi
