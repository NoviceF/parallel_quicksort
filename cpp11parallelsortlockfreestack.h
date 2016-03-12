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

                    if (!m_jobIsDone.load())
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

            size_t pivotPos = 0;
            quicksort(input, indcs.begin, indcs.end, &pivotPos);
            Indeces lowerPart = { indcs.begin, pivotPos ? pivotPos - 1 : 0 };
            Indeces higherPart = { pivotPos + 1, indcs.end };

            if (lowerPart.begin < lowerPart.end)
                m_stack.push(lowerPart);

            if (higherPart.begin < higherPart.end)
            {
                if (m_threadsWaitingCounter.load())
                {
                    std::unique_lock<std::mutex> lock(m_condMutex);

                    if (m_threadsWaitingCounter.load())
                    {
                        m_stack.push(higherPart);
                        m_condVar.notify_one();
                    }
                    else
                    {
                        lock.unlock();
                        localStack.push(higherPart);
                    }
                }
                else
                {
                    localStack.push(higherPart);
                }
            }
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

//            T const pivot = input[indcs.begin];

//            using vecIter = typename std::vector<T>::iterator;
//            vecIter beginIter = input.begin() + indcs.begin;
//            vecIter endIter = (input.begin() + indcs.end) + 1;
//            vecIter middle1 = std::partition(beginIter, endIter,
//                                          [&](T const& t){ return t < pivot; });
//            vecIter middle2 = std::partition(middle1, endIter,
//                                          [&](T const& t){ return !(pivot < t); });
//            const size_t middle1Pos = middle1 - input.begin();
//            const size_t middle2Pos = middle2 - input.begin();
//            Indeces lowerPart = { indcs.begin, middle1Pos ? middle1Pos - 1 : 0};
//            Indeces higherPart = { middle2Pos, indcs.end };


#endif // CPP11PARALLELSORTLOCKFREESTACK_H
