#include <algorithm>
#include <functional>

#include <sys/stat.h>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

#include "blockingthreadsafestack.h"

int main(int argc, char* argv[])
{

    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());
    Logger logger;

////    sleep(1);
////    typedef CSort<int> csortInt;
////    SingleThreadSortTest<csortInt> csortTest(params);
////    csortTest.runTest(logger);
//    sleep(1);

    typedef STLSort<int> stlSortInt;
    SingleThreadSortTest<stlSortInt> stlSortTest(params);
    stlSortTest.runTest(logger);

    sleep(1);

    typedef PosixParallelSort<int> posixSortInt;
    MultiThreadSortTest<posixSortInt> posixSortTest(params);
    posixSortTest.runTest(logger);

//    sleep(1);

//    typedef Cpp11ParallelSortAsync<int> asyncSortInt;
//    MultiThreadSortTest<asyncSortInt> asyncSortTest(params);
//    asyncSortTest.runTest(logger);

    sleep(1);

    typedef Cpp11ParallelSortPartitioning<int> partitioningSortInt;
    MultiThreadSortTest<partitioningSortInt> partitioningSortTest(params);
    partitioningSortTest.runTest(logger);




//    logger.PrintTimeTable(csortInt::Name);
    logger.printTimeTable(stlSortInt::Name);
//    logger.PrintTimeTable(posixSortInt::Name);

    return 0;
}


