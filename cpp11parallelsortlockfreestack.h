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

#include "common.h"
#include "lockfreestack.h"
#include "sortbase.h"


template <typename T>
class Cpp11ParallelSortLockFreeStack : public MultiThreadSort<T>
{
private:
    struct Indeces
    {
        size_t begin;
        size_t end;
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
        m_stack.push({ 0, MultiThreadSort<T>::m_vecToSort.size() - 1 });
        MultiThreadSort<T>::initTask();
    }

    void sortRoutine()
    {
        Indeces indcs = { 0, 0 };
        // try process new interval from stack
        while (!m_stack.pop(indcs))
        {
            if (m_jobIsDone.load())
                return;

            if (m_threadsWaitingCounter.fetch_add(1) + 1
                    == MultiThreadSort<T>::threadsCount())
            {
                std::unique_lock<std::mutex> lock(m_condMutex);
                m_jobIsDone.store(true);
                m_condVar.notify_all();
                return;
            }

            std::unique_lock<std::mutex> lock(m_condMutex);

            if (!m_jobIsDone.load())
                m_condVar.wait(lock);

            m_threadsWaitingCounter.fetch_sub(1);
        }
        // sort getted interval
        if (indcs.begin >= indcs.end)
            return;

        std::vector<T>& input = MultiThreadSort<T>::m_vecToSort;
        T const& pivot = input[indcs.begin];

        auto divide_point = std::partition(input.begin() + indcs.begin,
                                           input.begin() + indcs.end,
                                           [&](T const& t){ return t < pivot; });
        Indeces lower_part = {
            indcs.begin, static_cast<size_t>((divide_point - 1) - input.begin()) };
        Indeces higher_part = {
            static_cast<size_t>((divide_point) - input.begin()), indcs.end };

        m_stack.push(lower_part);
        m_stack.push(higher_part);
    }

    void runTask() override
    {
        std::vector<std::thread> threads;
        ThreadsJoiner guard(threads);

        for (int i = 0; i < MultiThreadSort<T>::threadsCount(); ++i)
        {
            threads.emplace_back(std::thread(
                 &Cpp11ParallelSortLockFreeStack::sortRoutine, this));
        }
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
const std::string Cpp11ParallelSortLockFreeStack<T>::Name = "LFSQs";


#endif // CPP11PARALLELSORTLOCKFREESTACK_H
