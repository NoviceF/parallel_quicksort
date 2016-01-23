#include <algorithm>
#include <cassert>

#include "profiler.h"

/////////////////
//profiler impl//
/////////////////

Prof::Prof() /*: measValue_(0)*/
{
}

void Prof::StopAndPrint()
{
    StopMeas();
    PrintMeas();
}

double Prof::StopAndGetDifference()
{
    StopMeas();

    if (t1_ < t2_)
    {
        boost::posix_time::time_duration td(t2_ - t1_);
        double temp(td.total_milliseconds());

        return temp / 1000;
    }
    else
    {
        std::cout << "Can't measure time!" << std::endl;
    }

    return -1;
}

void Prof::StartMeas()
{
    t1_ = boost::posix_time::microsec_clock::local_time();
}

void Prof::StopMeas()
{
    t2_ = boost::posix_time::microsec_clock::local_time();
}

void Prof::PrintMeas() const
{
    if (t1_ < t2_)
    {
        boost::posix_time::time_duration td(t2_ - t1_);
        double temp(td.total_milliseconds());
        std::cout << std::endl << temp / 1000 << " seconds elapsed\n" << std::endl;
    }
    else
    {
        std::cout << "Can't measure time!" << std::endl;
    }
}

////////////////
//logger impl//
///////////////

const int Logger::PosOfStlMeasure;

struct MinMaxAvg
{
    double min;
    double max;
    double avg;
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

//void PrintTotalTime(std::pair<int, Values> pair, double stlAvg);

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

void Logger::CalcValues()
{
//    for (std::map<int, std::vector<double> >::iterator it = measureMap.begin();
//            it != measureMap.end(); ++it)
//    {
//        assert(!it->second.empty());

//        MinMaxAvg minMaxAvg = GetMinMaxAvg(it->second);
//        m_mapRes[it->first].min = minMaxAvg.min;
//        m_mapRes[it->first].max = minMaxAvg.max;
//        m_mapRes[it->first].avg = minMaxAvg.avg;
//    }
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

//void Logger::PrintTableHead(int vecSize, int loopCount)
//{
//    PrintLine();
//    std::cout << "      " << "vector size = " << vecSize << "; "
//            << "count of loop = " << loopCount << std::endl;
//    PrintLine();
//}

void Logger::PrintTimeTable()
{
    std::map<ElementsCount, std::map<ThreadsCount,
            std::pair<SortName, ExecutionTime> > > mapForPrint;

    for (auto nameIter = m_logMap.begin(); nameIter != m_logMap.end(); ++nameIter)
    {
        for (auto elementsCountIter = nameIter->second.begin();
             elementsCountIter != nameIter->second.end(); ++elementsCountIter)
        {
            for (auto threadsCountIter = elementsCountIter->second.begin();
                 threadsCountIter != elementsCountIter->second.end();
                 ++threadsCountIter)
            {
                mapForPrint[elementsCountIter->first][threadsCountIter->first]
                    = std::make_pair(nameIter->first, threadsCountIter->second);
            }
        }
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

//void PrintTotalTime(std::pair<int, Values> pair, double stlAvg)
//{
//    switch (pair.first)
//    {
//        case Logger::PosOfStlMeasure: std::cout << "STL impl" << "           ";
//            break;
//        default:
//            std::cout << pair.first << " threads impl" << "     ";

//    }

//    std::cout << std::fixed << std::setprecision(3);
//    std::cout << pair.second.min << "s     " << pair.second.max << "s     "
//              << pair.second.avg << "s ";

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
//}
