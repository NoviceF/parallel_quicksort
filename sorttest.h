#ifndef SORTTEST_H
#define SORTTEST_H

#include <memory>
#include <vector>

#include "commandpromptparser.h"
#include "sort.h"


inline void fillVectorFullRandom(std::vector<int>& vecToFill, const size_t vecSize)
{
    srand(time(NULL));

    for (size_t i = 0; i < vecSize; ++i)
    {
        vecToFill.push_back(rand() % vecSize);
    }
}

inline void fillVectorHalfRandom(std::vector<int>& vecToFill, const size_t vecSize)
{
    for (size_t i = 0; i < vecSize / 2; ++i)
    {
        vecToFill.push_back(i);
    }

    srand(time(NULL));

    for (size_t i = vecSize / 2 + 1; i < vecSize; ++i)
    {
        vecToFill.push_back(rand() % 100);
    }
}

class ISortTest
{
public:
    virtual ~ISortTest() = 0;

    virtual void runTest(Logger& logger) = 0;
};

inline ISortTest::~ISortTest()
{
}

template <class T>
class SingleThreadSortTest : public ISortTest
{
public:
    SingleThreadSortTest(const ParsedParams& testParams)
        : m_testParams(testParams)
    {
    }

    void runTest(Logger& logger) override
    {
        size_t vecSize = m_testParams.firstVecSize;

        do
        {
            logger.setElementsCount(vecSize);
            std::vector<int> vecToSort;
            m_testParams.type == fullSort
                    ? fillVectorFullRandom(vecToSort, vecSize)
                    : fillVectorHalfRandom(vecToSort, vecSize);

            T sort(vecToSort, logger);
            setupSort(sort);

            for (size_t i = 0; i < m_testParams.loopCount; ++i)
            {
                std::random_shuffle(vecToSort.begin(), vecToSort.end());
                sort.makeSort();
            }

            vecSize += m_testParams.incStep;
        }
        while (vecSize < m_testParams.lastVecSize + 1);
    }

private:
    virtual void setupSort(T&)
    {
    }

protected:
    const ParsedParams& m_testParams;
};

template <class T>
class MultiThreadSortTest : public SingleThreadSortTest<T>
{
public:
    MultiThreadSortTest(const ParsedParams& testParams)
        : SingleThreadSortTest<T>(testParams)
    {
    }

private:
    void setupSort(T& sort) override
    {
        sort.setThreadsCount(MultiThreadSortTest<T>::m_testParams.thrCountInTest);
    }
};

#endif // SORTTEST_H
