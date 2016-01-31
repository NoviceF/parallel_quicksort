#ifndef CPP11PARALLELSORTASYNC_H
#define CPP11PARALLELSORTASYNC_H

#include <string>
#include <vector>

#include "sortbase.h"

template <typename T>
class Cpp11ParallelSortAsync : public MultiThreadSort<T>
{
public:
    Cpp11ParallelSortAsync(std::vector<T>& vecToSort)
        : SortBase<T>(vecToSort)
    {}

    void doSort() override;
};


#endif // CPP11PARALLELSORTASYNC_H
