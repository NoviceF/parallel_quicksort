#ifndef CPP11PARALLELSORTLOCKFREESTACK_H
#define CPP11PARALLELSORTLOCKFREESTACK_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "lockfreestack.h"
#include "sortbase.h"


template <typename T>
class Cpp11ParallelSortLockFreeStack : public MultiThreadSort<T>
{
private:
    struct Indeces
    {
        const size_t begin;
        const size_t end;
    };

public:
    static const std::string Name;

    Cpp11ParallelSortLockFreeStack(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
        , m_jobIsDone(false)
        , m_threadsWaitingCounter(0)
    {}

    void initTask() override
    {
        assert(std::atomic<Indeces>().is_lock_free());
        MultiThreadSort<T>::initTask();
    }

    void sort(const Indeces& indcs)
    {
        int valueFromStack = 0;

        while (!m_stack.pop(valueFromStack))
        {
            if (m_threadsWaitingCounter.fetch_add(1) + 1 == m_pusherThreadsCount)
                m_jobIsDone.store(true);
        }

        if (indcs.begin >= indcs.end)
           return;

        std::vector<T>& input = MultiThreadSort<T>::m_vecToSort;
        T const& pivot = input[indcs.begin];

        auto divide_point = std::partition(input.begin() + indcs.begin,
                                           input.begin() + indcs.end,
                                           [&](T const& t){ return t < pivot; });
        Indeces lower_part = { indcs.begin, (divide_point - 1) - input.begin() };
        Indeces higher_part = { (divide_point) - input.begin(), indcs.end };

        m_stack.push(lower_part);
//        std::future<std::list<T> > new_lower(
//                    std::async(&asyncQSort<T>, std::move(lower_part)));

//        auto new_higher(asyncQSort(std::move(input)));

//        result.splice(result.end(), new_higher);
//        result.splice(result.begin(), new_lower.get());
//        return result;
    }

    void runTask() override
    {
    }

//    void finalizeTask() override
//    {
//    }

    std::string name() const override
    {
        return Cpp11ParallelSortLockFreeStack::Name;
    }

private:
    LockFreeStack<Indeces> m_stack;
    std::mutex m_condMutex;
    std::condition_variable m_condVar;
    std::atomic<bool> m_jobIsDone;
    std::atomic<size_t> m_threadsWaitingCounter;
};

template <typename T>
const std::string Cpp11ParallelSortLockFreeStack<T>::Name = "Cpp11P";


#endif // CPP11PARALLELSORTLOCKFREESTACK_H
