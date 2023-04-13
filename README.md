# HWT - balanced tree

This project is a task on course "Uses and applications of C++ language" by [K.I.Vladimirov](https://github.com/tilir). The task was to implement a class representing balanced tree that finds k-th smallest element (K-query) and the number of elements smaller than the given one (N-query) in O(log n) time.

## Requirements

The following application have to be installed:
- CMake of version 3.15 (or higher)

## How to install
```bash
git clone git@github.com:KetchuppOfficial/Tree.git
cd Tree
```

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Tree/)

### 1) Build the project
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build [--target <tgt>]
```
**tgt** can be **driver**, **generator** or **ans_generator**. The **driver** is a program that recieves queries (insert, K-queries, N-queries) from stdin and prints the answers to them on stdout. The **generator** is a program the generates those queries randomly. The **ans_generator** a program that does the same as **driver** but uses std::set.

If --target option is omitted, all targets will be built.

I strongly suggest to build the project in *Release* mode. That is because all function that somehow affect the structure of the tree contain checking whether the invariants have been violated. Such checking takes much time.

## How to run unit tests
```bash
ctest --test-dir build
```

## How to run end-to-end tests

If you want to run some tests on my tree, go to [test/end_to_end](/test/end_to_end/) directory. There you will find a special script **checker.sh** provided for such purpose.

Let **N** be the number of queries, **wI** - weight of insert queris, **wK** - weight of K-queries, **wN** - weight of N-queries, then command sequence:
```bash
./checker.sh N wI wK wN
```
generates **N** random queries approximately **wI/N** of which are insert queries, **wK/N** are K-queries and **wN/N** are N-queries. The test is saved in **N.test**. After that this script runs **ans_generator**, gets answers that are supposed to be correct and saves them in file **N.ans**. Then, **driver** does the same. The results are saved in **N.res**. Finally, files **N.ans** and **N.res** are compared. If they differ, then this test is considered "failed". It is considered "passed" otherwise.

P.s. all above mentioned files locate in test/end_to_end/data directory.

P.p.s. **driver** and **ans_generator** measure the time spent on running a test. This information is saved in **driver.info** and **ans.info** files.

# Behold... Advanced Red-black tree

![dump](/images/dump_example.png)
