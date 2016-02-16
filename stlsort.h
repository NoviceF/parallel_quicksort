#ifndef STLSORT_H
#define STLSORT_H

#include <string>
#include <vector>

#include "logger.h"
#include "profiler.h"
#include "sortbase.h"


template <typename T>
class STLSort : public SingleThreadSort<T>
{
public:
    static const std::string Name;

    STLSort(std::vector<T>& vecToSort, Logger& logger)
        : SingleThreadSort<T>(vecToSort, logger)
    {}

    void runTask() override
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


#endif // STLSORT_H
