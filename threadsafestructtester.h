#ifndef THREADSAFESTRUCTTESTER_H
#define THREADSAFESTRUCTTESTER_H


template <typename T, template <typename> class CONT = LockFreeStack>
class ThreadSafeStackTester
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
    ThreadSafeStackTester(size_t numbersSetSize,
                        size_t writersCount,
                        size_t readersCount)
        : m_setSize(numbersSetSize)
        , m_pusherThreadsCount(writersCount)
        , m_getterThreadsCount(readersCount)
        , m_writersReadyCounter(0)
        , m_readersReadyCounter(0)
        , m_ready(m_go.get_future())
    {}

    void operator()()
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
                std::cout << "test execution exception cause " << ex.what()
                          << std::endl;
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

        //    std::cout << "testLockFreeStack finish succesfully." << std::endl;
    }

private:
    void pusherThread(std::vector<int>::const_iterator begin,
                      std::vector<int>::const_iterator end,
                      std::shared_future<void> ready)
    {
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
    const std::vector<int>& m_source;
    std::vector<int> m_result;

    const size_t m_setSize;
    const size_t m_pusherThreadsCount;
    const size_t m_getterThreadsCount;

    std::atomic<size_t> m_writersReadyCounter;
    std::atomic<size_t> m_readersReadyCounter;

    std::promise<void> m_writersReady;
    std::promise<void> m_readersReady;
    std::promise<void> m_go;

    std::shared_future<void> m_ready;
};


#endif // THREADSAFESTRUCTTESTER_H
