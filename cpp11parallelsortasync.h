// @author Anthony Williams ISBN: 978-5-94074-448-1

#ifndef CPP11PARALLELSORTASYNC_H
#define CPP11PARALLELSORTASYNC_H

#include <algorithm>
#include <future>
#include <list>
#include <string>
#include <vector>

#include "sortbase.h"


template <typename T>
std::list<T> asyncQSort(std::list<T> input)
{
    if (input.empty())
        return input;

    std::list<T> result;

    result.splice(result.begin(), input, input.begin());
    T const& pivot =* result.begin();

    auto divide_point = std::partition(input.begin(), input.end(),
                                       [&](T const& t){ return t < pivot; });
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    std::future<std::list<T> > new_lower(
                std::async(&asyncQSort<T>, std::move(lower_part)));

    auto new_higher(asyncQSort(std::move(input)));

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get());
    return result;
}

template <typename T>
class Cpp11ParallelSortAsync : public MultiThreadSort<T>
{
public:
    static const std::string Name;

    Cpp11ParallelSortAsync(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
    {}

    void initTask() override
    {
        m_listToSort.assign(MultiThreadSort<T>::m_vecToSort.begin(),
                            MultiThreadSort<T>::m_vecToSort.end());

        MultiThreadSort<T>::initTask();
    }

    void runTask() override
    {
        m_listToSort = asyncQSort(m_listToSort);
    }

    void finalizeTask() override
    {
        MultiThreadSort<T>::m_logger.saveTime(
                    MultiThreadSort<T>::m_profiler.StopAndGetDifference());

        MultiThreadSort<T>::m_vecToSort.assign(m_listToSort.begin(),
                                               m_listToSort.end());

        if (!std::is_sorted(MultiThreadSort<T>::m_vecToSort.begin(),
                            MultiThreadSort<T>::m_vecToSort.end()))
        {
            throw std::runtime_error("SortBase::doPostSort: vec is not sorted!");
        }
    }

    std::string name() const override
    {
        return Cpp11ParallelSortAsync::Name;
    }

private:
    std::list<T> m_listToSort;
};

template <typename T>
const std::string Cpp11ParallelSortAsync<T>::Name = "Async";


#endif // CPP11PARALLELSORTASYNC_H
