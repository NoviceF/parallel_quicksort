#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <vector>

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

#endif // COMMON_H

