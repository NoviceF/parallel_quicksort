#ifndef SORTTEST_H
#define SORTTEST_H

#include <memory>
#include <vector>

#include "commandpromptparser.h"
#include "sort.h"


class ISortTest
{
public:
    virtual ~ISortTest() = 0;

    virtual void runTest() = 0;
};

template <typename T>
class SingleThreadSortTest : public ISortTest
{
public:
    SingleThreadSortTest(ISort* sort, const ParsedParams& testParams);

    ~SingleThreadSortTest() override;

    void runTest() override;

private:
    virtual void loopCore(std::vector<int>& vecToSort, Logger& logger);

protected:
    std::shared_ptr<ISort> m_sort;
    const ParsedParams& m_testParams;
};

#endif // SORTTEST_H
