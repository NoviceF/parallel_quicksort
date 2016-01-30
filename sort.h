#ifndef SORT_H
#define SORT_H

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

#include "logger.h"
#include "profiler.h"
#include "posixsortmanager.h"

class ISort
{
public:
    virtual void makeSort() = 0;
    virtual ~ISort() = 0;

    virtual std::string name() const = 0;
};

inline ISort::~ISort()
{
}

template <typename T>
class SortBase : public ISort
{
public:
    SortBase(std::vector<T>& vecToSort, Logger& logger)
        : m_vecToSort(vecToSort)
        , m_logger(logger)
    {
        if (m_vecToSort.empty())
            throw std::runtime_error("SortBase<T>::SortBas: vec to sort is empty!");
    }

    virtual ~SortBase() {}

    void makeSort()
    {
        doPreSort();
        doSort();
        doPostSort();
    }

protected:
    virtual void doPreSort()
    {
        m_logger.setSortName(name());
        m_profiler.StartMeas();
    }

    virtual void doSort() = 0;

    virtual void doPostSort()
    {
        m_logger.saveTime(m_profiler.StopAndGetDifference());

        if (!std::is_sorted(m_vecToSort.begin(), m_vecToSort.end()))
            throw std::runtime_error("SortBase::doPostSort: vec is not sorted!");
    }

protected:
    std::vector<T>& m_vecToSort;
    Logger& m_logger;
    Prof m_profiler;
};

template <typename T>
class SingleThreadSort : public SortBase<T>
{
public:
    SingleThreadSort(std::vector<T>& vecToSort, Logger& logger)
        : SortBase<T>(vecToSort, logger)
    {
    }
};

template <typename T>
class MultiThreadSort : public SortBase<T>
{
public:
    MultiThreadSort(std::vector<T>& vecToSort, Logger& logger)
        : SortBase<T>(vecToSort, logger)
        , m_threadsCount(0)
    {
    }

    int threadsCount() const
    {
        return m_threadsCount;
    }

    void setThreadsCount(int threadsCount)
    {
        m_threadsCount = threadsCount;
    }

private:
    void doPreSort() override
    {
        if (!m_threadsCount)
        {
            throw std::runtime_error(
                        "MultiThreadSort::doPreSort: need to set threads count!");
        }

        SortBase<T>::m_logger.setThreadCount(m_threadsCount);

        SortBase<T>::doPreSort();
    }

private:
    int m_threadsCount;
};

template <typename T>
class CSort : public SingleThreadSort<T>
{
public:
    static const std::string Name;

    CSort(std::vector<T>& vecToSort, Logger& logger)
        : SingleThreadSort<T>(vecToSort, logger)
    {}

    void doSort() override
    {
        qsort(
            SortBase<T>::m_vecToSort.data(),
            SortBase<T>::m_vecToSort.size(),
            sizeof(T),
            [](const void* first,
                const void* second)
            {
                const T* arg1 = static_cast<const T*>(first);
                const T* arg2 = static_cast<const T*>(second);
                if (*arg1 < *arg2) return -1;
                else if (*arg1 == *arg2) return 0;
                else return 1;
            }
        );
    }

    std::string name() const override
    {
        return CSort::Name;
    }
};

template <typename T>
const std::string CSort<T>::Name = "C";

template <typename T>
class STLSort : public SingleThreadSort<T>
{
public:
    static const std::string Name;

    STLSort(std::vector<T>& vecToSort, Logger& logger)
        : SingleThreadSort<T>(vecToSort, logger)
    {}

    void doSort() override
    {
        std::sort(
            SortBase<T>::m_vecToSort.begin(),
            SortBase<T>::m_vecToSort.end()
        );
    }

    std::string name() const override
    {
        return STLSort::Name;
    }
};

template <typename T>
const std::string STLSort<T>::Name = "STL";


template <typename T>
class PosixParallelSort : public MultiThreadSort<T>
{
public:
    static const std::string Name;

    PosixParallelSort(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
    {}

    void doSort() override
    {
        const size_t vecSize = SortBase<T>::m_vecToSort.size();

        if (vecSize < 2)
            return;

        const size_t threadsCount = MultiThreadSort<T>::threadsCount();
        Manager manager(SortBase<T>::m_vecToSort, threadsCount);
        manager.addWork(vecSize);

        std::vector<pthread_t> threads(threadsCount);
        std::vector<SortingTask> tasks(threadsCount);

        for (int i = 0; i < threads.size(); ++i)
        {
            tasks[i].id = i;
            tasks[i].mn = &manager;
//            tasks[i].dataType = type;

            int status = pthread_create(&threads[i], NULL, f, &tasks[i]);
            if (status != 0)
            {
                std::cout << "thread starting failed" << std::endl;
            }
        }

        for (int i = 0; i < threadsCount; ++i)
        {
            pthread_join(threads[i], NULL);
        }
    }

    std::string name() const override
    {
        return PosixParallelSort::Name;
    }

//    void doSortString(int threadsCount, Logger& logger, std::string path)
//    {
//        stringType stringImpl;

//        switch (threadsCount)
//        {
//            case 1: stringImpl = pOnethread;
//                break;
//            case 2: stringImpl = pTwothreads;
//                break;
//            case 4: stringImpl = pFourthreads;
//                break;
//            default:
//            {
//                cout << "Unknown count of threads!";
//                return;
//            }
//        }
//        //"COPYING"
//        std::ifstream inputFile(path);
//        std::string str;
//        Manager mn1(threadsCount);

//        while (inputFile >> str) mn1.vecStr.push_back(str);

//        mn1.addWork(mn1.vecStr.size());

//        //    cout << "vecsize = " << vecSize << endl;

//        pthread_t *threads = new pthread_t[threadsCount];

//        Prof prof;
//        startAll(threads, threadsCount, mn1, SortingTask::string);
//        logger.GetTime(stringImpl, prof.StopAndGet());

//        CheckSorted(mn1.vecStr, mn1.vecStr.size());
//        //    for (int i = 0; i < mn1.vecStr.size(); ++i)
//        //    {
//        //        cout << mn1.vecStr[i] << " ";
//        //    }

//        delete []threads;
//    }
};

template <typename T>
const std::string PosixParallelSort<T>::Name = "Posix";

template <typename T>
class Cpp11ParallelSort : public MultiThreadSort<T>
{
public:
    Cpp11ParallelSort(std::vector<T>& vecToSort)
        : SortBase<T>(vecToSort)
    {}

    void doSort() override;
};


#endif // SORT_H
