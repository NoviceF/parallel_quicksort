#include <algorithm>
#include <chrono>
#include <thread>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

#include "blockingthreadsafestack.h"
#include "lockfreestack.h"
#include "metainfo.h"
#include "profiler.h"

#include "threadsafestructtester.h"

void algorithmsTest(const ParsedParams& params, Logger& logger);
void structsTest(const ParsedParams& params, Logger& logger);

int main(int argc, char* argv[])
{
    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());
    Logger logger;




//    algorithmsTest(params, logger);
    structsTest(params, logger);

    logger.printTimeTable(CSort<int>::Name);
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
    runTest<SingleThreadTestRunner, STLSort<int> >(params, logger);
    runTest<MultiThreadTestRunner, PosixParallelSort<int> >(params, logger);
    runTest<MultiThreadTestRunner, Cpp11ParallelSortAsync<int> >(params, logger);
    runTest<MultiThreadTestRunner, Cpp11ParallelSortPartitioning<int> >(
                params, logger);
}

void structsTest(const ParsedParams& params, Logger& logger)
{
    runTest<MultiThreadTestRunner,
            ThreadSafeStackTester<int, ThreadsafeStack> >(params, logger);
    runTest<MultiThreadTestRunner,
            ThreadSafeStackTester<int, LockFreeStack> >(params, logger);
}
