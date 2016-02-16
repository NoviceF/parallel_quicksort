#ifndef TASKBASE_H
#define TASKBASE_H

#include <stdexcept>
#include <vector>

#include "logger.h"
#include "profiler.h"


class ITask
{
public:
    virtual void operator()() = 0;
    virtual ~ITask() = 0;

    virtual std::string name() const = 0;
};

inline ITask::~ITask()
{
}

template <typename T>
class TaskBase : public ITask
{
public:
    TaskBase(std::vector<T>& vecToSort, Logger& logger)
        : m_vecToSort(vecToSort)
        , m_logger(logger)
    {
        if (m_vecToSort.empty())
            throw std::runtime_error("TaskBase<T>::TaskBase: vec to sort is empty!");
    }

    virtual ~TaskBase() {}

    void operator()()
    {
        initTask();
        runTask();
        finalizeTask();
    }

protected:
    virtual void initTask()
    {
        m_logger.setSortName(name());
        m_profiler.StartMeas();
    }

    virtual void runTask() = 0;

    virtual void finalizeTask()
    {
        m_logger.saveTime(m_profiler.StopAndGetDifference());
    }

protected:
    std::vector<T>& m_vecToSort;
    Logger& m_logger;
    Prof m_profiler;
};


#endif // TASKBASE_H

