#include <algorithm>
#include <stdexcept>

#include <sort.h>

template <typename T>
SortBase<T>::SortBase(std::vector<T>& vecToSort)
    : m_vecToSort(vecToSort)
{}

template <typename T>
SortBase<T>::~SortBase()
{

}

template <typename T>
void SortBase<T>::doPreSortCheck()
{
   if (m_vecToSort.empty())
       throw std::runtime_error("SortBase::doPreSortCheck: vec to sort is empty!");
}

template <typename T>
void SortBase<T>::doPostSortCheck()
{
   if (!std::is_sorted(m_vecToSort.begin(), m_vecToSort.end()))
       throw std::runtime_error("SortBase::doPostSortCheck: vec is not sorted!");
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
CSort<T>::CSort(std::vector<T>& vecToSort)
    : SortBase<T>(vecToSort)
{}

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
