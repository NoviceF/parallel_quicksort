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

MinMaxAvg GetMinMaxAvg(std::vector<double> vec)
{
    assert(!vec.empty());

    double minValue = vec[0];
    double maxValue = vec[0];
    double avgValue = vec[0];

    for (int i = 0; i < static_cast<int> (vec.size()); ++i)
    {
        if (vec[i] < minValue) minValue = vec[i];
        if (vec[i] > maxValue) maxValue = vec[i];
        avgValue += vec[i];
    }

    avgValue /= static_cast<int> (vec.size());

    return { minValue, maxValue, avgValue };
}


void PrintLine(int lengh = 54)
{
    for (int i = 0; i < lengh; ++i)
    {
        std::cout << "-";
    }

    std::cout << std::endl;
}

void PrintTableHead(int vecSize)
{
    PrintLine();
    std::cout << "                  " << "vector size = " << vecSize << std::endl;
    PrintLine();
}

void PrintTotalTime(std::map<Logger::SortName,  std::map<Logger::ThreadsCount,
        SortResult > > resultsMap, const std::string& pivotSortName)
{
    for (auto nameIter = resultsMap.begin(); nameIter != resultsMap.end();
         ++nameIter)
    {
        for (auto threadsCountIter = nameIter->second.begin();
             threadsCountIter != nameIter->second.end(); ++threadsCountIter)
        {
            std::cout << nameIter->first << "    "
                      << (threadsCountIter->first ? threadsCountIter->first : 1)
                      << (threadsCountIter->first == 1
                         ? " threads impl"
                         : " thread  impl")
                       << "     ";

            const MinMaxAvg& minMaxAvg = threadsCountIter->second.minMaxAvg;

            std::cout << std::fixed << std::setprecision(3);
            std::cout << minMaxAvg.min << "s     "
                      << minMaxAvg.max << "s     "
                      << minMaxAvg.avg << "s     "
                      << threadsCountIter->second.iterationCount
                      << " iterations.";

            std::cout << std::endl;
        }
    }



//    if (!pair.first) std::cout << "STL" << std::endl;
//    else if (pair.second.avg > stlAvg && stlAvg)
//    {
//        std::cout << "-" <<
//            static_cast<int> ((pair.second.avg - stlAvg) / stlAvg * 100)
//            << "%" << std::endl;
//    }
//    else if (pair.second.avg <= stlAvg && stlAvg)
//    {
//        std::cout << "+" <<
//            static_cast<int> ((stlAvg - pair.second.avg) / pair.second.avg * 100)
//            << "%" << std::endl;
//    }
}

} // namespace

Logger::Logger()
    : m_sortName("default sort name")
    , m_elementsCount(0)
    , m_threadsCount(0)
{
}

void Logger::Reset()
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

void Logger::SaveTime(double executionTime)
{
    m_logMap[m_sortName][m_elementsCount][m_threadsCount].push_back(executionTime);
}

void Logger::PrintTimeTable(const std::string& pivotSortName)
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
                            GetMinMaxAvg(threadsCountIter->second)
                            };
            }
        }
    }

    for (auto elementsCountIter = mapForPrint.begin();
         elementsCountIter != mapForPrint.end(); ++elementsCountIter)
    {
        PrintTableHead(elementsCountIter->first);

        PrintTotalTime(elementsCountIter->second, pivotSortName);

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
