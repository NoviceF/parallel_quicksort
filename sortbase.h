#ifndef SORTBASE_H
#define SORTBASE_H

#include <algorithm>
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


#endif // SORTBASE_H
