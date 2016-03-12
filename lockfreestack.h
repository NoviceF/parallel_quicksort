// @author of LockFreeStack is Anthony Williams ISBN: 978-5-94074-448-1

#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <iostream>
#include <memory>
#include <mutex>
#include <future>
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

    bool pop(T& value)
    {
        CountedNodePtr oldHead = head.load(std::memory_order_relaxed);

        for(;;)
        {
            increaseHeadCount(oldHead);
            Node* const ptr = oldHead.ptr;

            if (!ptr)
            {
                return false;
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

                value = *res;

                return true;
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


template <>
class LockFreeStack<int>
{
    struct Node;

    struct CountedNodePtr
    {
        int externalCount;
        Node* ptr;
    };

    struct Node
    {
        int data;
        std::atomic<int> internalCount;
        CountedNodePtr next;

        Node(int dataArg)
            : data((dataArg))
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

    void push(int data)
    {
        CountedNodePtr newNode;
        newNode.ptr = new Node(data);
        newNode.externalCount = 1;
        newNode.ptr->next = head.load(std::memory_order_relaxed);

        while (!head.compare_exchange_weak(newNode.ptr->next, newNode,
                                           std::memory_order_release,
                                           std::memory_order_relaxed));
    }

    std::shared_ptr<int> pop()
    {
        CountedNodePtr oldHead = head.load(std::memory_order_relaxed);

        for(;;)
        {
            increaseHeadCount(oldHead);
            Node* const ptr = oldHead.ptr;

            if (!ptr)
            {
                return std::shared_ptr<int>();
            }

            if (head.compare_exchange_strong(oldHead, ptr->next,
                                             std::memory_order_relaxed))
            {
                std::shared_ptr<int> res(std::make_shared<int>(ptr->data));
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

    bool pop(int& value)
    {
        CountedNodePtr oldHead = head.load(std::memory_order_relaxed);

        for(;;)
        {
            increaseHeadCount(oldHead);
            Node* const ptr = oldHead.ptr;

            if (!ptr)
            {
                return false;
            }

            if (head.compare_exchange_strong(oldHead, ptr->next,
                                             std::memory_order_relaxed))
            {
                int const res = ptr->data;
                int const countIncrease = oldHead.externalCount - 2;

                if (ptr->internalCount.fetch_add(
                            countIncrease, std::memory_order_release)
                        == -countIncrease)
                {
                    delete ptr;
                }

                value = res;

                return true;
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


#endif // LOCKFREESTACK_H
