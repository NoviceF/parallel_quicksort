#ifndef SORTTEST_H
#define SORTTEST_H

#include <memory>
#include <vector>

#include "commandpromptparser.h"
#include "sort.h"

void fillVectorFullRandom(std::vector<int>& vecToFill);
void fillVectorHalfRandom(std::vector<int>& vecToFill);


class ISortTest
{
public:
    virtual ~ISortTest() = 0;

    virtual void runTest() = 0;
};

inline ISortTest::~ISortTest()
{
}

template <class T>
class SingleThreadSortTest : public ISortTest
{
public:
    SingleThreadSortTest(const ParsedParams& testParams);

    ~SingleThreadSortTest() override
    {
    }

    void runTest() override;

private:
    virtual void loopCore(std::vector<int>& vecToSort, Logger& logger);

protected:
    const ParsedParams& m_testParams;
};

template <class T>
SingleThreadSortTest<T>::SingleThreadSortTest(const ParsedParams& testParams)
    : m_testParams(testParams)
{
}

template <class T>
void SingleThreadSortTest<T>::runTest()
{
    Logger logger;

    size_t vecSize = m_testParams.firstVecSize;

    do
    {
        std::vector<int> vecToSort;
        m_testParams.type == fullSort
                ? fillVectorFullRandom(vecToSort)
                : fillVectorHalfRandom(vecToSort);

        loopCore(vecToSort, logger);

        vecSize += m_testParams.incStep;
    }
    while (vecSize < m_testParams.lastVecSize + 1);

    logger.PrintTimeTable();
}

template <class T>
void SingleThreadSortTest<T>::loopCore(std::vector<int>& vecToSort, Logger& logger)
{
    T sort(vecToSort, logger);

    for (size_t i = 0; i < m_testParams.loopCount; ++i)
    {
        sort.makeSort();
    }
}


#endif // SORTTEST_H
