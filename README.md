# olimp_structures

1. Dymanic Graph solve [Fully Dynamic Connectivity Problem](https://en.wikipedia.org/wiki/Dynamic_connectivity) online by O(log^2 n) for each query

2. [Set](Set.h) is based on AvlTree and almost equal std::set, but faseter in 4 times

3. [Fixed Set](FixedSet.h) is realisation of [Perfect Hash function](https://en.wikipedia.org/wiki/Perfect_hash_function)

4. ["Map for poor"](MapForPoor.h) is standart runtime analysing problem.

    * Insert query by O(log n) amortized
    * Contains query by O(log^2 n)
    * No erase

5. [Inplace merge sort](InplaceMergeSort.h) using additional O(log n) memory for recursuion.

6. [Radix sort](RadixSortUInt32.h) can sort 10^7 elements in 0.5s
