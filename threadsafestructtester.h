#ifndef THREADSAFESTRUCTTESTER_H
#define THREADSAFESTRUCTTESTER_H

#include <cassert>
#include <stdexcept>

#include "blockingthreadsafestack.h"
#include "lockfreestack.h"
#include "metainfo.h"
#include "taskbase.h"

template <typename T, template <typename> class CONT = LockFreeStack>
class ThreadSafeStackTester : public TaskBase<T>
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
    ThreadSafeStackTester(std::vector<T>& vecToSort, Logger& logger)
        : TaskBase<T>(vecToSort, logger)
        , m_source(vecToSort)
        , m_setSize(m_source.size())
        , m_pusherThreadsCount(0)
        , m_getterThreadsCount(0)
        , m_writersReadyCounter(0)
        , m_readersReadyCounter(0)
        , m_ready(m_go.get_future())
    {}

    void setThreadsCount(size_t threadsCount)
    {
        m_pusherThreadsCount = threadsCount;
        m_getterThreadsCount = threadsCount;
    }

    std::string name() const override;

    // TaskBase interface
protected:
    void runTask() override
    {
        std::sort(m_source.begin(), m_source.end());

        {
            try
            {
                std::vector<std::thread> threads;
                ThreadsJoiner guard(threads);
                initAndStartPushThreads(threads, m_source);
                initAndStartGetterThreads(threads, m_getterThreadsCount);

                m_writersReady.get_future().wait();
                m_readersReady.get_future().wait();
                TaskBase<T>::initTask();
                m_go.set_value();
            }
            catch (const std::exception& ex)
            {
                m_go.set_value();
                std::cout << "test execution exception cause " << ex.what()
                          << std::endl;
            }
            catch (...)
            {
                m_go.set_value();
                std::cout << "test execution exception" << std::endl;
            }

            TaskBase<T>::finalizeTask();
        }

        std::sort(m_result.begin(), m_result.end());

        if (m_source != m_result)
            throw std::runtime_error("Error working with lockfreestack");

        //    std::cout << "testLockFreeStack finish succesfully." << std::endl;
    }

    void initTask() override
    {
    }
    void finalizeTask() override
    {
    }

private:
    void pusherThread(std::vector<int>::const_iterator begin,
                      std::vector<int>::const_iterator end,
                      std::shared_future<void> ready)
    {
        assert(m_pusherThreadsCount);

        if (m_writersReadyCounter.fetch_add(1) + 1 == m_pusherThreadsCount)
            m_writersReady.set_value();

        ready.wait();

        for (std::vector<int>::const_iterator it = begin; it != end; ++it)
        {
            m_threadSafeStack.push(*it);
        }
    }

    void getterThread(std::shared_future<void> ready)
    {
        assert(m_getterThreadsCount);

        if (m_readersReadyCounter.fetch_add(1) + 1 == m_getterThreadsCount)
            m_readersReady.set_value();

        ready.wait();

        bool isResultFull = false;

        while (true)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                isResultFull = m_result.size() == m_setSize;
            }
            std::shared_ptr<int> value = m_threadSafeStack.pop();

            if (!value && !isResultFull)
                continue;
            else if (!value)
                break;

            std::lock_guard<std::mutex> lock(m_mutex);
            m_result.push_back(*value);
        }
    }

    void initAndStartPushThreads(std::vector<std::thread>& threads,
                                 const std::vector<int>& sourceSet)
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
                                     &ThreadSafeStackTester::pusherThread, this,
                                     partBegin, partEnd, m_ready)
                                 );
        }
    }

    void initAndStartGetterThreads(std::vector<std::thread>& threads, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            threads.emplace_back(std::thread(
                                     &ThreadSafeStackTester::getterThread, this,
                                     m_ready)
                                 );
        }
    }

private:
    CONT<T> m_threadSafeStack;

    std::mutex m_mutex;
    std::vector<int>& m_source;
    std::vector<int> m_result;

    const size_t m_setSize;
    size_t m_pusherThreadsCount;
    size_t m_getterThreadsCount;

    std::atomic<size_t> m_writersReadyCounter;
    std::atomic<size_t> m_readersReadyCounter;

    std::promise<void> m_writersReady;
    std::promise<void> m_readersReady;
    std::promise<void> m_go;

    std::shared_future<void> m_ready;

};

template <typename T, template <typename> class CONT>
std::string ThreadSafeStackTester<T, CONT>::name() const
{
    throw std::runtime_error("Not implemented!");
}

template <>
std::string ThreadSafeStackTester<int, LockFreeStack>::name() const
{
    return "TSS"; // thread safe stack
}

template <>
std::string ThreadSafeStackTester<int, ThreadsafeStack>::name() const
{
    return "BS"; // blocking stack
}

//TaskMetaManager::registerTaskMeta<ThreadSafeStackTester<int> >(
//        TaskMetaManager::structs, make_simple_creator<ThreadSafeStackTester>);

#endif // THREADSAFESTRUCTTESTER_H

