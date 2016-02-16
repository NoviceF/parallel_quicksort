#ifndef SORTBASE_H
#define SORTBASE_H

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "logger.h"
#include "taskbase.h"


template <typename T>
class SortBase : public TaskBase<T>
{
public:
    SortBase(std::vector<T>& vecToSort, Logger& logger)
        : TaskBase<T>(vecToSort, logger)
    {
    }

protected:
    void finalizeTask() override
    {
        TaskBase<T>::finalizeTask();

        if (!std::is_sorted(TaskBase<T>::m_vecToSort.begin(),
                            TaskBase<T>::m_vecToSort.end()))
        {
            throw std::runtime_error("SortBase::doPostSort: vec is not sorted!");
        }
    }
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

protected:
    void initTask() override
    {
        if (!m_threadsCount)
        {
            throw std::runtime_error(
                        "MultiThreadSort::doPreSort: need to set threads count!");
        }

        SortBase<T>::m_logger.setThreadCount(m_threadsCount);

        SortBase<T>::initTask();
    }

private:
    int m_threadsCount;
};


#endif // SORTBASE_H
