#include <algorithm>
#include <functional>

#include <sys/stat.h>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

int main(int argc, char* argv[])
{
    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());
    Logger logger;

    typedef CSort<int> csortInt;
    SingleThreadSortTest<csortInt> csortTest(params);
    csortTest.runTest(logger);

    typedef STLSort<int> stlSortInt;
    SingleThreadSortTest<stlSortInt> stlSortTest(params);
    stlSortTest.runTest(logger);

    typedef PosixParallelSort<int> posixSortInt;
    MultiThreadSortTest<posixSortInt> posixSortTest(params);
    posixSortTest.runTest(logger);



    logger.PrintTimeTable(csortInt::Name);
//    logger.PrintTimeTable(posixSortInt::Name);

    return 0;
}


