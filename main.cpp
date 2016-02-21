#include <algorithm>
#include <chrono>
#include <functional>
#include <stdexcept>
//#include <sys/stat.h>
#include <thread>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

#include "blockingthreadsafestack.h"
#include "lockfreestack.h"
#include "profiler.h"

#include "threadsafestructtester.h"

void algorithmsTest(const ParsedParams& params, Logger& logger);
void structsTest(const ParsedParams& params, Logger& logger);

//class ITest
//{
//public:
//    virtual ~ITest() = 0;

//    virtual void runTest(Logger& logger) = 0;
//};

//inline ITest::~ITest()
//{
//}

//template <class T>
//class SingleThreadTestRunner : public ITest
//{
//public:
//    SingleThreadTestRunner(const ParsedParams& testParams)
////        : m_testParams(testParams)
//    {
//    }

//    ~SingleThreadTestRunner() override {}

//    void runTest(Logger& logger) override
//    {
////        size_t vecSize = m_testParams.firstVecSize;

////        do
////        {
////            logger.setElementsCount(vecSize);
////            std::vector<int> vecToTest;
////            m_testParams.type == fullSort
////                    ? fillVectorFullRandom(vecToTest, vecSize)
////                    : fillVectorHalfRandom(vecToTest, vecSize);

////            for (size_t i = 0; i < m_testParams.loopCount; ++i)
////            {
////                T test(vecToTest, logger);
////                setupTest(test);

////                std::random_shuffle(vecToTest.begin(), vecToTest.end());
////                test();
////            }

////            vecSize += m_testParams.incStep;
////        }
////        while (vecSize < m_testParams.lastVecSize + 1);
//    }

//private:
//    virtual void setupTest(T&)
//    {
//    }

//protected:
////    const ParsedParams& m_testParams;
//};

int main(int argc, char* argv[])
{
    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());
    Logger logger;

    typedef CSort<int> csortInt;

    algorithmsTest(params, logger);
    structsTest(params, logger);


    logger.printTimeTable(csortInt::Name);
//    logger.printTimeTable(stlSortInt::Name);
//    logger.PrintTimeTable(posixSortInt::Name);


    return 0;
}

template <template <typename> class TEST_RUNNER,
          typename TEST>
static void runTest(const ParsedParams& params, Logger& logger)
{
        TEST_RUNNER<TEST> test(params);
        test.runTest(logger);

        std::this_thread::sleep_for(std::chrono::seconds(1));
}

void algorithmsTest(const ParsedParams& params, Logger& logger)
{
    runTest<SingleThreadTestRunner, CSort<int> >(params, logger);

    {
//        typedef CSort<int> csortInt;
//        SingleThreadTestRunner<int> csortTest(params);
//        csortTest.runTest(logger);
    }

//    {
//        SingleThreadTestRunner<csortInt> csortTest(params);
//        csortTest.runTest(logger);
//    }
//    sleep(1);

//    typedef STLSort<int> stlSortInt;
//    SingleThreadSortTest<stlSortInt> stlSortTest(params);
//    stlSortTest.runTest(logger);

//    sleep(1);

//    typedef PosixParallelSort<int> posixSortInt;
//    MultiThreadTestRunner<posixSortInt> posixSortTest(params);
//    posixSortTest.runTest(logger);

//    sleep(1);


//    typedef Cpp11ParallelSortAsync<int> asyncSortInt;
//    MultiThreadSortTest<asyncSortInt> asyncSortTest(params);
//    asyncSortTest.runTest(logger);

//    sleep(1);

//    typedef Cpp11ParallelSortPartitioning<int> partitioningSortInt;
//    MultiThreadTestRunner<partitioningSortInt> partitioningSortTest(params);
//    partitioningSortTest.runTest(logger);
}

void structsTest(const ParsedParams& params, Logger& logger)
{

//    runTest<ThreadSafeStackTester>(params, logger);

    runTest<MultiThreadTestRunner,
            ThreadSafeStackTester<int, ThreadsafeStack> >(params, logger);

//    {
//        typedef threadsafestacktester<int> lockfreeintstacktester;
//        multithreadtestrunner<lockfreeintstacktester> test(params);
//        test.runtest(logger);
//    }

//    {
//        typedef threadsafestacktester<int, threadsafestack> blockingstacktester;
//        multithreadtestrunner<blockingstacktester> test(params);
//        test.runtest(logger);
//    }
}
