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
#include "quicksortimpl.h"
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
//        MultiThreadSort<T>::m_vecToSort = { 5, 2, 0, 1, 6, 3};
//        MultiThreadSort<T>::m_vecToSort = { 5, 5, 5, 1, 6, 6, 1, 6, 4};
        m_stack.push({ 0, MultiThreadSort<T>::m_vecToSort.size() - 1 });
        MultiThreadSort<T>::initTask();
    }

    void sortRoutine()
    {
        std::stack<Indeces> localStack;

        while (true)
        {
            Indeces indcs = { 0, 0 };

            if (localStack.empty())
            {
                // try process new interval from stack
                while (!m_stack.pop(indcs))
                {
                    if (m_jobIsDone.load())
                        return;

                    std::unique_lock<std::mutex> lock(m_condMutex);

                    if (m_threadsWaitingCounter.fetch_add(1) + 1
                            == MultiThreadSort<T>::threadsCount())
                    {
                        assert(std::is_sorted(
                                   MultiThreadSort<T>::m_vecToSort.begin(),
                                   MultiThreadSort<T>::m_vecToSort.end()));
                        m_jobIsDone.store(true);
                        m_condVar.notify_all();
                        return;
                    }

//                    if (!m_jobIsDone.load())
                        m_condVar.wait(lock);

                    m_threadsWaitingCounter.fetch_sub(1);
                }
            }
            else
            {
               indcs = localStack.top();
               localStack.pop();
            }
            // sort getted interval
            assert(indcs.begin < indcs.end);

            std::vector<T>& input = MultiThreadSort<T>::m_vecToSort;

            size_t middle1Pos = 0;
            size_t middle2Pos = 0;
            partitionStl(input, indcs.begin, indcs.end, middle1Pos, middle2Pos);
            Indeces lowerPart = { indcs.begin, middle1Pos ? middle1Pos - 1 : 0 };
            Indeces higherPart = { middle2Pos, indcs.end };

            auto fillStacks = [&](const Indeces& ind)
            {
                if (ind.begin < ind.end)
                {
                    // the value MultiThreadSort<T>::threadsCount() / 2
                    // is chosen empirically
                    if (localStack.empty()
                            || m_threadsWaitingCounter.load()
                               < MultiThreadSort<T>::threadsCount() / 2)
                    {
                        localStack.push(ind);
                    }
                    else
                    {
                        m_stack.push(ind);
                        m_condVar.notify_one();
                    }
                }
            };

            fillStacks(lowerPart);
            fillStacks(higherPart);
        }
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
const std::string Cpp11ParallelSortLockFreeStack<T>::Name = "LFS";


#endif // CPP11PARALLELSORTLOCKFREESTACK_H
