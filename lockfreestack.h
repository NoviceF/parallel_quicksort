// @author of LockFreeStack is Anthony Williams ISBN: 978-5-94074-448-1

#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <vector>


template <typename T>
class LockFreeStack
{
private:
    struct Node;

    struct CountedNodePtr
    {
        int externalCount;
        Node* ptr;
    };

    struct Node
    {
        std::shared_ptr<T> data;
        std::atomic<int> internalCount;
        CountedNodePtr next;

        Node(T const& dataArg)
            : data(std::make_shared<T>(dataArg))
            , internalCount(0)
        {}

    };

    std::atomic<CountedNodePtr> head;

    void increaseHeadCount(CountedNodePtr& oldCounter)
    {
        CountedNodePtr newCounter;

        do
        {
            newCounter = oldCounter;
            ++newCounter.externalCount;
        }
        while (!head.compare_exchange_strong(oldCounter, newCounter,
                                             std::memory_order_acquire,
                                             std::memory_order_relaxed)) ;

        oldCounter.externalCount = newCounter.externalCount;
    }

public:
    LockFreeStack()
        : head({0, nullptr})
    {}

    ~LockFreeStack()
    {
        while (pop());
    }

    void push(T const& data)
    {
        CountedNodePtr newNode;
        newNode.ptr = new Node(data);
        newNode.externalCount = 1;
        newNode.ptr->next = head.load(std::memory_order_relaxed);

        while (!head.compare_exchange_weak(newNode.ptr->next, newNode,
                                           std::memory_order_release,
                                           std::memory_order_relaxed));
    }

    std::shared_ptr<T> pop()
    {
        CountedNodePtr oldHead = head.load(std::memory_order_relaxed);

        for(;;)
        {
            increaseHeadCount(oldHead);
            Node* const ptr = oldHead.ptr;

            if (!ptr)
            {
                return std::shared_ptr<T>();
            }

            if (head.compare_exchange_strong(oldHead, ptr->next,
                                             std::memory_order_relaxed))
            {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int const countIncrease = oldHead.externalCount - 2;

                if (ptr->internalCount.fetch_add(
                            countIncrease, std::memory_order_release)
                        == -countIncrease)
                {
                    delete ptr;
                }

                return res;
            }
            else if (ptr->internalCount.fetch_add(
                         -1, std::memory_order_relaxed) == 1)
            {
                ptr->internalCount.load(std::memory_order_acquire);
                delete ptr;
            }
        }
    }
};

class LockFreeStackTester
{
    class ThreadsJoiner
    {
    public:
        explicit ThreadsJoiner(std::vector<std::thread>& threads)
            : m_threads(threads)
        {}

        ~ThreadsJoiner()
        {
            for (size_t i = 0; i < m_threads.size(); ++i)
            {
                if (m_threads[i].joinable())
                    m_threads[i].join();
            }
        }

    private:
        std::vector<std::thread>& m_threads;
    };

public:
    LockFreeStackTester(size_t numbersSetSize,
                        size_t writersCount,
                        size_t readersCount)
        : m_setSize(numbersSetSize)
        , m_pusherThreadsCount(writersCount)
        , m_getterThreadsCount(readersCount)
    {}

    void testLockFreeStack();

private:
    void pusherThread(std::multiset<int>::iterator begin,
                      std::multiset<int>::iterator end);
    void getterThread();
    void initAndStartPushThreads(std::vector<std::thread>& threads,
                         std::multiset<int> sourceSet);
    void initAndStartGetterThreads(std::vector<std::thread>& threads, size_t count);

private:
    LockFreeStack<int> m_lockFreeStack;
    std::mutex m_mutex;
    std::multiset<int> m_result;
    const size_t m_setSize;
    const size_t m_pusherThreadsCount;
    const size_t m_getterThreadsCount;
};


#endif // LOCKFREESTACK_H
