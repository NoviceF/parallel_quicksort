#include <algorithm>
#include <functional>

#include <sys/stat.h>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

//#include "blockingthreadsafestack.h"
#include "lockfreestack.h"

int main(int argc, char* argv[])
{
    for (int i = 0; i < 10000; ++i)
    {
        const size_t elementCount = 10000;
        const size_t writersCount = 2;
        const size_t readersCount = 2;
        LockFreeStackTester tester(elementCount, writersCount, readersCount);
        tester.testLockFreeStack();
    }












/*
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

//    typedef PosixParallelSort<int> posixSortInt;
//    MultiThreadSortTest<posixSortInt> posixSortTest(params);
//    posixSortTest.runTest(logger);

//    sleep(1);


    typedef Cpp11ParallelSortAsync<int> asyncSortInt;
    MultiThreadSortTest<asyncSortInt> asyncSortTest(params);
    asyncSortTest.runTest(logger);

    sleep(1);

    typedef Cpp11ParallelSortPartitioning<int> partitioningSortInt;
    MultiThreadSortTest<partitioningSortInt> partitioningSortTest(params);
    partitioningSortTest.runTest(logger);

    logger.printTimeTable(csortInt::Name);
//    logger.printTimeTable(stlSortInt::Name);
//    logger.PrintTimeTable(posixSortInt::Name);
*/

    return 0;
}


