#ifndef SORT_H
#define SORT_H

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

#include "logger.h"
#include "profiler.h"

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

private:
    virtual void doPreSort()
    {
        m_logger.setSortName(name());
        m_profiler.StartMeas();
    }

    virtual void doSort() = 0;

    virtual void doPostSort()
    {
        m_logger.SaveTime(m_profiler.StopAndGetDifference());

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
    MultiThreadSort()
        : m_threadsCount(0)
    {
    }
    ~MultiThreadSort();

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
    PosixParallelSort(std::vector<T>& vecToSort)
        : SortBase<T>(vecToSort)
    {}

    void doSort() override;
};

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
