#include "lockfreestack.h"


void LockFreeStackTester::testLockFreeStack()
{
    std::multiset<int> numberSet;

    std::srand(std::time(0));

    for (size_t i = 0; i < m_setSize; ++i)
    {
        numberSet.insert(std::rand() % m_setSize);
    }

    {
        std::vector<std::thread> threads;
        ThreadsJoiner guard(threads);
        initAndStartPushThreads(threads, numberSet);
        initAndStartGetterThreads(threads, m_pusherThreadsCount);
    }

    if (numberSet != m_result)
        throw std::runtime_error("Error working with lockfreestack");

    std::cout << "testLockFreeStack finish succesfully." << std::endl;
}

void LockFreeStackTester::pusherThread(std::multiset<int>::iterator begin,
                                       std::multiset<int>::iterator end)
{
    for (std::multiset<int>::iterator it = begin; it != end; ++it)
    {
        m_lockFreeStack.push(*it);
    }
}

void LockFreeStackTester::getterThread()
{
    bool isResultFull = false;

    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            isResultFull = m_result.size() == m_setSize;
        }
        std::shared_ptr<int> value = m_lockFreeStack.pop();

        if (!value && !isResultFull)
            continue;
        else if (!value)
            break;

        std::lock_guard<std::mutex> lock(m_mutex);
        m_result.insert(*value);
    }
}

void LockFreeStackTester::initAndStartPushThreads(std::vector<std::thread>& threads, std::multiset<int> sourceSet)
{
    assert(!sourceSet.empty() && m_pusherThreadsCount);

    const size_t partsSize = sourceSet.size() / m_pusherThreadsCount;

    std::multiset<int>::const_iterator partBegin = sourceSet.begin();
    std::multiset<int>::const_iterator partEnd = partBegin;

    size_t remaining = partsSize;

    while (remaining >= partsSize)
    {
        remaining = (std::distance(partEnd, sourceSet.end()));

        if (!remaining)
            break;

        partBegin = partEnd;

        if (remaining < partsSize)
        {
            std::advance(partEnd, remaining);
        }
        else
        {
            std::advance(partEnd, partsSize);
        }

        threads.emplace_back(
            std::thread(&LockFreeStackTester::pusherThread, this, partBegin, partEnd));
    }
}

void LockFreeStackTester::initAndStartGetterThreads(std::vector<std::thread>& threads, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        threads.emplace_back(std::thread(&LockFreeStackTester::getterThread, this));
    }
}
