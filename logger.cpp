#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>

#include "logger.h"

namespace {

struct MinMaxAvg
{
    double min;
    double max;
    double avg;
};

struct SortResult
{
    Logger::IterationCount iterationCount;
    MinMaxAvg minMaxAvg;
};

MinMaxAvg getMinMaxAvg(const std::vector<double>& vec)
{
    assert(!vec.empty());

    double minValue = vec[0];
    double maxValue = vec[0];
    double avgValue = vec[0];

    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i] < minValue) minValue = vec[i];
        if (vec[i] > maxValue) maxValue = vec[i];
    }

    avgValue = (maxValue + minValue) / 2;

    return { minValue, maxValue, avgValue };
}


void printLine(int lengh = 72)
{
    for (int i = 0; i < lengh; ++i)
    {
        std::cout << "-";
    }

    std::cout << std::endl;
}

void printSpaces(size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        std::cout << " ";
    }
}

void printTableHead(int vecSize)
{
    printLine();
    printSpaces(26);
    std::cout << "vector size = " << vecSize << std::endl;
    printLine();
    const size_t lengthForName = 5;
    const size_t fillingSpacesCount = 4;

    const std::string name("Name");
    std::cout << name;
    printSpaces(lengthForName - name.length() + fillingSpacesCount);

    const std::string threads("Threads");
    std::cout << threads;
    const size_t threadsSpacesCount = 6;
    printSpaces(fillingSpacesCount - (threads.length() - threadsSpacesCount));

    const std::string min("Min");
    std::cout << min;
    const size_t elapsedValueInMsSpaces = 6;
    printSpaces(elapsedValueInMsSpaces - min.length() + fillingSpacesCount);

    const std::string max("Max");
    std::cout << max;
    printSpaces(elapsedValueInMsSpaces - max.length() + fillingSpacesCount);

    const std::string avg("Avg");
    std::cout << avg;
    printSpaces(elapsedValueInMsSpaces - max.length() + fillingSpacesCount);

    const std::string iterationCount("IterCount");
    std::cout << iterationCount;
    const size_t iterationCountSpaces = 7;
    printSpaces(fillingSpacesCount - (iterationCount.length() - iterationCountSpaces));

    const std::string differenceInPercent("Difference");
    std::cout << differenceInPercent;

    std::cout << std::endl;
    printLine();
}

void printTotalTime(std::map<Logger::SortName,  std::map<Logger::ThreadsCount,
        SortResult > > resultsMap, const std::string& pivotSortName)
{
    const size_t threadCountForPivot = 0;
    const double pivotAvg = resultsMap[pivotSortName]
        [threadCountForPivot].minMaxAvg.avg;

    for (auto nameIter = resultsMap.begin(); nameIter != resultsMap.end();
         ++nameIter)
    {
        for (auto threadsCountIter = nameIter->second.begin();
             threadsCountIter != nameIter->second.end(); ++threadsCountIter)
        {
            std::cout << nameIter->first;

            const size_t lengthForName = 5;

            if (nameIter->first.size() < lengthForName)
            {
                printSpaces(lengthForName - nameIter->first.size());
            }

            std::cout <<  "    "
                      << (threadsCountIter->first ? threadsCountIter->first : 1)
                      << (threadsCountIter->first == 1
                         ? " thr "
                         : " thrs")
                       << "    ";

            const MinMaxAvg& minMaxAvg = threadsCountIter->second.minMaxAvg;

            std::cout << std::fixed << std::setprecision(3);
            std::cout << minMaxAvg.min / 1000 << "s    "
                      << minMaxAvg.max / 1000 << "s    "
                      << minMaxAvg.avg / 1000 << "s    "
                      << threadsCountIter->second.iterationCount
                      << " iters    ";

            const double currentAvg = minMaxAvg.avg;
//            assert(pivotAvg);

            if (currentAvg > pivotAvg)
            {
                std::cout << "-"
                    << static_cast<double> ((currentAvg - pivotAvg)
                                         / pivotAvg * 100)
                    << "%" << std::endl;
            }
            else if (currentAvg <= pivotAvg)
            {
                std::cout << "+" <<
                    static_cast<double> ((pivotAvg - currentAvg)
                                      / pivotAvg * 100)
                    << "%" << std::endl;
            }
        }
    }

    std::cout << std::endl;
}

} // namespace

Logger::Logger()
    : m_sortName("default sort name")
    , m_elementsCount(0)
    , m_threadsCount(0)
{
}

void Logger::reset()
{
//    measureMap.clear();
//    m_mapRes.clear();
}

std::string Logger::sortName() const
{
    return m_sortName;
}

void Logger::setSortName(const std::string& sortName)
{
    m_sortName = sortName;
}

size_t Logger::elementsCount() const
{
    return m_elementsCount;
}

void Logger::setElementsCount(const size_t& elementsCount)
{
    m_elementsCount = elementsCount;
}

size_t Logger::threadCount() const
{
    return m_threadsCount;
}

void Logger::setThreadCount(size_t threadCount)
{
    m_threadsCount = threadCount;
}

//const std::map<int, std::vector<double> >& Logger::measureMap() const
//{
//    return m_map;
//}

void Logger::saveTime(double executionTime)
{
    m_logMap[m_sortName][m_elementsCount][m_threadsCount].push_back(executionTime);
}

void Logger::printTimeTable(const std::string& pivotSortName)
{

    std::map<ElementsCount, std::map<SortName, std::map<ThreadsCount,
                SortResult > > > mapForPrint;

    for (auto nameIter = m_logMap.begin(); nameIter != m_logMap.end(); ++nameIter)
    {
        for (auto elementsCountIter = nameIter->second.begin();
             elementsCountIter != nameIter->second.end(); ++elementsCountIter)
        {
            for (auto threadsCountIter = elementsCountIter->second.begin();
                 threadsCountIter != elementsCountIter->second.end();
                 ++threadsCountIter)
            {
                mapForPrint[elementsCountIter->first][nameIter->first]
                        [threadsCountIter->first] = {
                            threadsCountIter->second.size(),
                            getMinMaxAvg(threadsCountIter->second)
                            };
            }
        }
    }

    for (auto elementsCountIter = mapForPrint.begin();
         elementsCountIter != mapForPrint.end(); ++elementsCountIter)
    {
        printTableHead(elementsCountIter->first);

        printTotalTime(elementsCountIter->second, pivotSortName);

//        for (auto nameIter = elementsCountIter->second.begin();
//             nameIter != elementsCountIter->second.end(); ++nameIter)
//        {
//            PrintTotalTime(nameIter->second, pivotSort);
//        }

    }


    // TODO: change data structure for supporting new log format;
    //    PrintTableHead();
    //    CalcValues();

    //    std::cout << std::endl << "Impl type" << "          "
    //            << "Min        Max        Avg" << std::endl << std::endl;
    //    std::for_each(m_mapRes.begin(), m_mapRes.end(),
    //        std::bind2nd((std::ptr_fun(PrintTotalTime)), m_mapRes[PosOfStlMeasure].avg)
    //        );
}
