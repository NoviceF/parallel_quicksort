#include "lockfreestack.h"


void LockFreeStackTester::testLockFreeStack()
{
    std::vector<int> numberSet;

    std::srand(std::time(0));

    for (size_t i = 0; i < m_setSize; ++i)
    {
        numberSet.push_back(std::rand() % m_setSize);
    }

    std::sort(numberSet.begin(), numberSet.end());

    {
        try
        {
            std::vector<std::thread> threads;
            ThreadsJoiner guard(threads);
            initAndStartPushThreads(threads, numberSet);
            initAndStartGetterThreads(threads, m_getterThreadsCount);

            m_writersReady.get_future().wait();
            m_readersReady.get_future().wait();
            m_go.set_value();
        }
        catch (const std::exception& ex)
        {
            m_go.set_value();
            std::cout << "test execution exception cause " << ex.what() << std::endl;
        }
        catch (...)
        {
            m_go.set_value();
            std::cout << "test execution exception" << std::endl;
        }
    }

    std::sort(m_result.begin(), m_result.end());

    if (numberSet != m_result)
        throw std::runtime_error("Error working with lockfreestack");

    std::cout << "testLockFreeStack finish succesfully." << std::endl;
}

void LockFreeStackTester::pusherThread(std::vector<int>::const_iterator begin,
                                       std::vector<int>::const_iterator end,
                                       std::shared_future<void> ready)
{
    m_writersReadyCounter.fetch_add(1);

    if (m_writersReadyCounter.load() == m_pusherThreadsCount)
         m_writersReady.set_value();

    ready.wait();

    for (std::vector<int>::const_iterator it = begin; it != end; ++it)
    {
        m_lockFreeStack.push(*it);
    }
}

void LockFreeStackTester::getterThread(std::shared_future<void> ready)
{
    m_readersReadyCounter.fetch_add(1);

    if (m_readersReadyCounter.load() == m_getterThreadsCount)
         m_readersReady.set_value();

    ready.wait();

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
        m_result.push_back(*value);
    }
}

void LockFreeStackTester::initAndStartPushThreads(
        std::vector<std::thread>& threads, const std::vector<int>& sourceSet)
{
    assert(!sourceSet.empty() && m_pusherThreadsCount);

    const size_t partsSize = sourceSet.size() / m_pusherThreadsCount;

    std::vector<int>::const_iterator partBegin = sourceSet.begin();
    std::vector<int>::const_iterator partEnd = partBegin;

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

        threads.emplace_back(std::thread(
                                 &LockFreeStackTester::pusherThread, this,
                                 partBegin, partEnd, m_ready)
                             );
    }
}

void LockFreeStackTester::initAndStartGetterThreads(
        std::vector<std::thread>& threads, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        threads.emplace_back(std::thread(
                                 &LockFreeStackTester::getterThread, this, m_ready)
                             );
    }
}
