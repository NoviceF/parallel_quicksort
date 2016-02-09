// @author Anthony Williams ISBN: 978-5-94074-448-1

#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <algorithm>
#include <atomic>
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

static LockFreeStack<int> s_lockFreeStack;
static std::mutex s_mutex;
static std::multiset<int> s_result;

void pusherThread(std::multiset<int>::iterator begin,
                  std::multiset<int>::iterator end)
{
    for (std::multiset<int>::iterator it = begin; it != end; ++it)
    {
        s_lockFreeStack.push(*it);
    }
}


void getterThread()
{
    while (std::shared_ptr<int> value = s_lockFreeStack.pop())
    {
        if (value)
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            s_result.insert(*value);
        }
    }
}

void testLockFreeStack()
{
    std::multiset<int> numberSet;

    const int setSize = 100;

    std::srand(std::time(0));

    for (int i = 0; i < setSize; ++i)
    {
        numberSet.insert(std::rand() % setSize);
    }

    const int threadsCount = 4;

    std::vector<std::thread> threads(threadsCount);

    ThreadsJoiner(threads);
}


#endif // LOCKFREESTACK_H
