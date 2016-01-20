#include "sorttest.h"


void fillVectorFullRandom(std::vector<int>& vecToFill)
{
    srand(time(NULL));
    size_t vecSize = vecToFill.size();

    for (size_t i = 0; i < vecSize; ++i)
    {
        vecToFill.push_back(rand() % vecSize);
    }
}


void fillVectorHalfRandom(std::vector<int>& vecToFill)
{
    size_t vecSize = vecToFill.size();

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

////////////////////////////////////////////////////////////////////////////////

template <typename T>
SingleThreadSortTest<T>::SingleThreadSortTest(ISort* sort, const ParsedParams& testParams)
    : m_sort(sort)
    , m_testParams(testParams)
{
}

template <typename T>
SingleThreadSortTest<T>::~SingleThreadSortTest()
{
}

template <typename T>
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

template <typename T>
void SingleThreadSortTest<T>::loopCore(std::vector<int>& vecToSort, Logger& logger)
{
    T sort(vecToSort, logger);

    for (size_t i = 0; i < m_testParams.loopCount; ++i)
    {
        sort.makeSort();
    }
}

////////////////////////////////////////////////////////////////////////////////
