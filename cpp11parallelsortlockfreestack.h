#ifndef CPP11PARALLELSORTLOCKFREESTACK_H
#define CPP11PARALLELSORTLOCKFREESTACK_H

#include <algorithm>
#include <future>
#include <list>
#include <string>
#include <vector>

#include "sortbase.h"


template <typename T>
class Cpp11ParallelSortLockFreeStack : public MultiThreadSort<T>
{
public:
    static const std::string Name;

    Cpp11ParallelSortLockFreeStack(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
    {}

//    void initTask() override
//    {
//        MultiThreadSort<T>::initTask();
//    }

    void runTask() override
    {
    }

//    void finalizeTask() override
//    {
//    }

    std::string name() const override
    {
        return Cpp11ParallelSortAsync::Name;
    }
};

template <typename T>
const std::string Cpp11ParallelSortLockFreeStack<T>::Name = "Cpp11P";


#endif // CPP11PARALLELSORTLOCKFREESTACK_H
