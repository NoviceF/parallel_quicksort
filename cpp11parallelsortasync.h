// @author Anthony Williams ISBN: 978-5-94074-448-1

#ifndef CPP11PARALLELSORTASYNC_H
#define CPP11PARALLELSORTASYNC_H


template <typename T>
class Cpp11ParallelSortAsync : public MultiThreadSort<T>
{
public:
    static const std::string Name;

    Cpp11ParallelSortAsync(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
    {}

//    void doPreSort() override
//    {
//        m_listToSort.assign(MultiThreadSort<T>::m_vecToSort.begin(),
//                            MultiThreadSort<T>::m_vecToSort.end());

//        MultiThreadSort<T>::doPreSort();
//    }
//    void doSort() override
//    {
//        m_listToSort = parallelQuickSort(m_listToSort);
//    }

//    void doPostSort() override
//    {
//        MultiThreadSort<T>::m_logger.saveTime(
//                    MultiThreadSort<T>::m_profiler.StopAndGetDifference());

//        MultiThreadSort<T>::m_vecToSort.assign(m_listToSort.begin(),
//                                               m_listToSort.end());

//        if (!std::is_sorted(MultiThreadSort<T>::m_vecToSort.begin(),
//                            MultiThreadSort<T>::m_vecToSort.end()))
//        {
//            throw std::runtime_error("SortBase::doPostSort: vec is not sorted!");
//        }
//    }

//    std::string name() const override
//    {
//        return Cpp11ParallelSortAsync::Name;
//    }

private:
//    std::list<T> m_listToSort;
};

#endif // CPP11PARALLELSORTASYNC_H
