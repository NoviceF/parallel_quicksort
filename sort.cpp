#include <algorithm>
#include <stdexcept>

#include "profiler.h"
#include "sort.h"

void testSort(ISort& sort)
{
}

template <typename T>
SortBase<T>::SortBase(std::vector<T>& vecToSort, Logger& logger)
    : m_vecToSort(vecToSort)
    , m_logger(logger)
{
   if (m_vecToSort.empty())
       throw std::runtime_error("SortBase<T>::SortBas: vec to sort is empty!");
}

template <typename T>
SortBase<T>::~SortBase()
{

}

template <typename T>
void SortBase<T>::doPreSort()
{
   m_profiler.StartMeas();
}

template <typename T>
void SortBase<T>::doPostSort()
{
   m_logger.SaveTime(m_profiler.StopAndGetDifference());

   if (!std::is_sorted(m_vecToSort.begin(), m_vecToSort.end()))
       throw std::runtime_error("SortBase::doPostSort: vec is not sorted!");
}

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

template <typename T>
MultiThreadSort<T>::MultiThreadSort()
    : m_threadsCount(0)
{
}

template <typename T>
int MultiThreadSort<T>::threadsCount() const
{
    return m_threadsCount;
}

template <typename T>
void MultiThreadSort<T>::setThreadsCount(int threadsCount)
{
    m_threadsCount = threadsCount;
}


template <typename T>
void MultiThreadSort<T>::doPreSort()
{
    if (!m_threadsCount)
    {
        throw std::runtime_error(
                "MultiThreadSort::doPreSort: need to set threads count!");
    }

    SortBase<T>::doPreSort();
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
CSort<T>::CSort(std::vector<T>& vecToSort, Logger& logger)
    : SortBase<T>(vecToSort, logger)
{}

void CSort::doSort()
{

}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
STLSort<T>::STLSort(std::vector<T>& vecToSort)
    : SortBase<T>(vecToSort)
{}

//////////////////////////////////////////////////////////////////////////////////

template <typename T>
PosixParallelSort<T>::PosixParallelSort(std::vector<T>& vecToSort)
    : SortBase<T>(vecToSort)
{}

//////////////////////////////////////////////////////////////////////////////////

template <typename T>
Cpp11ParallelSort<T>::Cpp11ParallelSort(std::vector<T>& vecToSort)
    : SortBase<T>(vecToSort)
{}


