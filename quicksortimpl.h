#ifndef SORTIMPL_H
#define SORTIMPL_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>


template<typename T>
void exch(std::vector<T>& a, size_t x, size_t y)
{
    if (x != y)
    {
        T temp = a[x];
        a[x] = a[y];
        a[y] = temp;
    }
}

template<typename T>
int partition(std::vector<T>& a, size_t start, size_t end)
{
    int i = start - 1;
    int j = end;
    T v = a[end];

    for (;;)
    {
        while (a[++i] < v);
        while (v < a[--j]) if (j == start) break;
        if (i >= j) break;

        exch(a, i, j);
    }

    exch(a, i, end);

    return i;
}

template<typename T>
void quicksort(std::vector<T>& a, size_t start, size_t end, size_t* i)
{
    assert(i);

    if (end <= start)
    {
        std::cout << "in qsort end <= start" << std::endl;
        std::cout << "start = " << start << std::endl;
        std::cout << "i = " << *i << std::endl;
        std::cout << "end = " << end << std::endl;

        return;
    }

    *i = partition(a, start, end);

    return;
}

template <typename T>
void partitionStl(std::vector<T>& vec,
             size_t beginPos,
             size_t endPos,
             size_t& middle1Pos,
             size_t& middle2Pos)
{
            T const pivot = vec[beginPos];

            using vecIter = typename std::vector<T>::iterator;
            vecIter beginIter = vec.begin() + beginPos;
            vecIter endIter = (vec.begin() + endPos) + 1;
            vecIter middle1 = std::partition(beginIter, endIter,
                                          [&](T const& t){ return t < pivot; });
            vecIter middle2 = std::partition(middle1, endIter,
                                          [&](T const& t){ return !(pivot < t); });
            middle1Pos = middle1 - vec.begin();
            middle2Pos = middle2 - vec.begin();
}

#endif  /* SORTIMPL_H */
