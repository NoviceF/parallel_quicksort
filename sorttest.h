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

    ~SingleThreadSortTest() override
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

            loopCore(vecToSort, logger);

            vecSize += m_testParams.incStep;
        }
        while (vecSize < m_testParams.lastVecSize + 1);
    }

private:
    virtual void loopCore(std::vector<int>& vecToSort, Logger& logger)
    {
        T sort(vecToSort, logger);

        for (size_t i = 0; i < m_testParams.loopCount; ++i)
        {
            sort.makeSort();
        }
    }

protected:
    const ParsedParams& m_testParams;
};

#endif // SORTTEST_H
