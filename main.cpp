#include <algorithm>
#include <functional>

#include <stdexcept>
#include <sys/stat.h>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

#include "blockingthreadsafestack.h"
#include "lockfreestack.h"
#include "profiler.h"

#include "threadsafestructtester.h"


int main(int argc, char* argv[])
{
    try
    {
        const size_t elementCount = 1000000;
        const size_t writersCount = 4;
        const size_t readersCount = 4;
        const size_t iterationsCount = 1;

        {
            std::cout << "Test lock free stack.." << std::endl;
            Prof prof;
            prof.StartMeas();

            for (size_t i = 0; i < iterationsCount; ++i)
            {
                ThreadSafeStackTester<int> tester(elementCount, writersCount, readersCount);
                tester.testLockFreeStack();
            }

            prof.StopAndPrint();
        }

        {
            std::cout << "Test stack with blocking.." << std::endl;
            Prof prof;
            prof.StartMeas();

            for (size_t i = 0; i < iterationsCount; ++i)
            {
                ThreadSafeStackTester<int, ThreadsafeStack> tester(
                            elementCount, writersCount, readersCount);
                tester.testLockFreeStack();
            }

            prof.StopAndPrint();
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "Can't finish test, cause " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Can't finish test, cause don't no!" << std::endl;
    }











    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());
    Logger logger;

    typedef CSort<int> csortInt;
    SingleThreadSortTest<csortInt> csortTest(params);
    csortTest.runTest(logger);
    sleep(1);

//    typedef STLSort<int> stlSortInt;
//    SingleThreadSortTest<stlSortInt> stlSortTest(params);
//    stlSortTest.runTest(logger);

//    sleep(1);

    typedef PosixParallelSort<int> posixSortInt;
    MultiThreadSortTest<posixSortInt> posixSortTest(params);
    posixSortTest.runTest(logger);

    sleep(1);


//    typedef Cpp11ParallelSortAsync<int> asyncSortInt;
//    MultiThreadSortTest<asyncSortInt> asyncSortTest(params);
//    asyncSortTest.runTest(logger);

//    sleep(1);

//    typedef Cpp11ParallelSortPartitioning<int> partitioningSortInt;
//    MultiThreadSortTest<partitioningSortInt> partitioningSortTest(params);
//    partitioningSortTest.runTest(logger);

    logger.printTimeTable(csortInt::Name);
//    logger.printTimeTable(stlSortInt::Name);
//    logger.PrintTimeTable(posixSortInt::Name);


    return 0;
}


