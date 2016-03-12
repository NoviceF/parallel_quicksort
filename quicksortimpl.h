#ifndef SORTIMPL_H
#define SORTIMPL_H

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

#endif  /* SORTIMPL_H */
