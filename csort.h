#ifndef CSORT_H
#define CSORT_H

#include <string>
#include <vector>

#include "logger.h"
#include "metainfo.h"
#include "sortbase.h"


template <typename T>
class CSort : public SingleThreadSort<T>
{
public:
    static const std::string Name;

    CSort(std::vector<T>& vecToSort, Logger& logger)
        : SingleThreadSort<T>(vecToSort, logger)
    {}

    void runTask() override
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

static MetaRegistrator<CSort<int> > s_reg(TaskMetaManager::sorts);


#endif // CSORT_H
