// @author Anthony Williams ISBN: 978-5-94074-448-1


#ifndef BLOCKINGTHREADSAFESTACK_H
#define BLOCKINGTHREADSAFESTACK_H

#include <cassert>
#include <memory>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <thread>


class empty_stack : public std::exception {};

template <typename T>
class ThreadsafeStack
{
private:
    std::stack<T> data;
    mutable std::mutex m;

public:
    ThreadsafeStack(){}
    ThreadsafeStack(const ThreadsafeStack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    ThreadsafeStack& operator=(const ThreadsafeStack&) = delete;

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);

        if (data.empty())
            return std::shared_ptr<T>();

        std::shared_ptr<T> const res(
            std::make_shared<T>(std::move(data.top())));
        data.pop();

        return res;
    }

    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);

        if (data.empty())
            throw empty_stack();

        value = std::move(data.top());
        data.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

inline void testStack()
{
    ThreadsafeStack<int> stack;

    bool getEmptyStackException = false;

    try
    {
        int i = 0;
        stack.pop(i);
    }
    catch (const empty_stack& ex)
    {
        getEmptyStackException = true;
    }

    assert(getEmptyStackException);

    getEmptyStackException = false;

    try
    {

        std::shared_ptr<int> i = stack.pop();
    }
    catch (const empty_stack& ex)
    {
        getEmptyStackException = true;
    }

    assert(getEmptyStackException);

    stack.push(2);

    assert(!stack.empty());

    stack.push(3);

    int i = 0;

    stack.pop(i);

    assert(i == 3);

    std::shared_ptr<int> pI = stack.pop();

    assert(*pI == 2);
}

#endif // BLOCKINGTHREADSAFESTACK_H

