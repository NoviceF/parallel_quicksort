// @author Anthony Williams ISBN: 978-5-94074-448-1

#ifndef CPP11PARALLELSORTASYNC_H
#define CPP11PARALLELSORTASYNC_H

#include <future>
#include <list>
#include <string>
#include <thread>
#include <vector>

#include "lockfreestack.h"
#include "sortbase.h"


template <typename T>
struct Sorter
{
    struct ChunkToSort
    {
        std::list<T> data;
        std::promise<std::list<T> > promise;
    };


};

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
