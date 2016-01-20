#ifndef SORT_H
#define SORT_H

#include <vector>

#include "profiler.h"

class ISort
{
public:
    virtual void makeSort() = 0;
    virtual ~ISort() = 0;
};

template <typename T>
class SortBase : public ISort
{
public:
    SortBase(std::vector<T>& vecToSort, Logger& logger);
    virtual ~SortBase();

    void makeSort()
    {
        doPreSort();
        doSort();
        doPostSort();
    }

private:
    virtual void doPreSort();
    virtual void doSort() = 0;
    virtual void doPostSort();

protected:
    std::vector<T>& m_vecToSort;
    Logger& m_logger;
    Prof m_profiler;
};

template <typename T>
class SingleThreadSort : public SortBase<T>
{
};

template <typename T>
class MultiThreadSort : public SortBase<T>
{
public:
    MultiThreadSort();
    ~MultiThreadSort();

    int threadsCount() const;
    void setThreadsCount(int threadsCount);

private:
    void doPreSort() override;

private:
    int m_threadsCount;
};

template <typename T>
class CSort : public SingleThreadSort<T>
{
public:
    CSort(std::vector<T>& vecToSort, Logger& logger);
    void doSort() override;
};

template <typename T>
class STLSort : public SingleThreadSort<T>
{
public:
    STLSort(std::vector<T>& vecToSort);
    void doSort() override;
};

template <typename T>
class PosixParallelSort : public MultiThreadSort<T>
{
public:
    PosixParallelSort(std::vector<T>& vecToSort);
    void doSort() override;
};

template <typename T>
class Cpp11ParallelSort : public MultiThreadSort<T>
{
public:
    Cpp11ParallelSort(std::vector<T>& vecToSort);
    void doSort() override;
};


#endif // SORT_H
