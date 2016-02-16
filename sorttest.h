#ifndef SORTTEST_H
#define SORTTEST_H

#include <memory>
#include <vector>

#include "commandpromptparser.h"
#include "taskbase.h"


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

class ITest
{
public:
    virtual ~ITest() = 0;

    virtual void runTest(Logger& logger) = 0;
};

inline ITest::~ITest()
{
}

template <class T>
class SingleThreadTestRunner : public ITest
{
public:
    SingleThreadTestRunner(const ParsedParams& testParams)
        : m_testParams(testParams)
    {
    }

    ~SingleThreadTestRunner() override {}

    void runTest(Logger& logger) override
    {
        size_t vecSize = m_testParams.firstVecSize;

        do
        {
            logger.setElementsCount(vecSize);
            std::vector<int> vecToTest;
            m_testParams.type == fullSort
                    ? fillVectorFullRandom(vecToTest, vecSize)
                    : fillVectorHalfRandom(vecToTest, vecSize);

            T test(vecToTest, logger);
            setupTest(test);

            for (size_t i = 0; i < m_testParams.loopCount; ++i)
            {
                std::random_shuffle(vecToTest.begin(), vecToTest.end());
                test();
            }

            vecSize += m_testParams.incStep;
        }
        while (vecSize < m_testParams.lastVecSize + 1);
    }

private:
    virtual void setupTest(T&)
    {
    }

protected:
    const ParsedParams& m_testParams;
};

template <class T>
class MultiThreadTestRunner : public SingleThreadTestRunner<T>
{
public:
    MultiThreadTestRunner(const ParsedParams& testParams)
        : SingleThreadTestRunner<T>(testParams)
    {
    }

    ~MultiThreadTestRunner() override {}

private:
    void setupTest(T& test) override
    {
        test.setThreadsCount(MultiThreadTestRunner<T>::m_testParams.thrCountInTest);
    }
};

#endif // SORTTEST_H
