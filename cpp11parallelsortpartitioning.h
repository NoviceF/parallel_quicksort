// @author Anthony Williams ISBN: 978-5-94074-448-1

#ifndef CPP11PARALLELSORTPARTITIONING_H
#define CPP11PARALLELSORTPARTITIONING_H


#include <future>
#include <list>
#include <memory>
#include <string>
#include <thread>
#include <vector>

//#include "lockfreestack.h"
#include "blockingthreadsafestack.h"
#include "sortbase.h"


template <typename T>
struct Sorter
{
    struct ChunkToSort
    {
        std::list<T> data;
        std::shared_ptr<std::promise<std::list<T> > > promise;

        ChunkToSort()
            : promise(new std::promise<std::list<T> >()){}
    };

//    LockFreeStack<ChunkToSort> chunks;
    ThreadsafeStack<ChunkToSort> chunks;
    std::vector<std::thread> threads;
    unsigned const maxThreadCount;
    std::atomic<bool> endOfData;

    Sorter(int threadsCount)
        : maxThreadCount(threadsCount - 1)
        , endOfData(false)
    {}

    ~Sorter()
    {
        endOfData = true;

        for (size_t i = 0; i < threads.size(); ++i)
        {
            threads[i].join();
        }
    }

    void trySortChunck()
    {
        std::shared_ptr<ChunkToSort> chunk = chunks.pop();

        if (chunk)
        {
            sortChunk(chunk);
        }
    }

    std::list<T> doSort(std::list<T>& chunkData)
    {
        if (chunkData.empty())
            return chunkData;

        std::list<T> result;
        result.splice(result.begin(), chunkData, chunkData.begin());
        T const& partitionVal = *result.begin();

        typename std::list<T>::iterator dividePoint =
                std::partition(chunkData.begin(), chunkData.end(),
                               [&](T const& val) { return val < partitionVal; });

        ChunkToSort newLowerChunk;

        newLowerChunk.data.splice(newLowerChunk.data.end(),
                                  chunkData, chunkData.begin(),
                                  dividePoint);
        std::future<std::list<T> > newLower =
              newLowerChunk.promise->get_future();

//        chunks.push(std::move(newLowerChunk));
        chunks.push(newLowerChunk);

        if (threads.size() < maxThreadCount)
        {
            threads.push_back(std::move(std::thread(&Sorter<T>::sortThread, this)));
        }

        std::list<T> newHigher(doSort(chunkData));

        result.splice(result.end(), newHigher);

        while (newLower.wait_for(std::chrono::seconds(0))
               != std::future_status::ready)
        {
            trySortChunck();
        }

        result.splice(result.begin(), newLower.get());

        return result;
    }

    void sortChunk(std::shared_ptr<ChunkToSort> const& chunk)
    {
        chunk->promise->set_value(doSort(chunk->data));
    }

    void sortThread()
    {
        while (!endOfData)
        {
            trySortChunck();
            std::this_thread::yield();
        }
    }
};

template <typename T>
std::list<T> parallelQuickSort(std::list<T> input, int threadsCount)
{
    if (input.empty())
        return input;

    Sorter<T> s(threadsCount);

    return s.doSort(input);
}


template <typename T>
class Cpp11ParallelSortPartitioning : public MultiThreadSort<T>
{
public:
    static const std::string Name;

    Cpp11ParallelSortPartitioning(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
    {}

    void doPreSort() override
    {
        m_listToSort.assign(MultiThreadSort<T>::m_vecToSort.begin(),
                            MultiThreadSort<T>::m_vecToSort.end());

        MultiThreadSort<T>::doPreSort();
    }
    void doSort() override
    {
        m_listToSort = parallelQuickSort(
                    m_listToSort,
                    MultiThreadSort<T>::threadsCount()
                    );
    }

    void doPostSort() override
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
        return Cpp11ParallelSortPartitioning::Name;
    }

private:
    std::list<T> m_listToSort;
};

template <typename T>
const std::string Cpp11ParallelSortPartitioning<T>::Name = "Async";


#endif // CPP11PARALLELSORTPARTITIONING_H

