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

void PrintTotalTime(std::pair<int, Values> pair, double stlAvg);

Logger::Logger()
    : m_threadCount(0)
    , m_iteartionCount(0)
{
}

void Logger::Reset()
{
//    measureMap.clear();
//    m_mapRes.clear();
}

void Logger::GetValues()
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
int Logger::iteartionCount() const
{
    return m_iteartionCount;
}

void Logger::setIteartionCount(int iteartionCount)
{
    m_iteartionCount = iteartionCount;
}

int Logger::threadCount() const
{
    return m_threadCount;
}

void Logger::setThreadCount(int threadCount)
{
    m_threadCount = threadCount;
}

const std::map<int, std::vector<double> >& Logger::measureMap() const
{
    return m_map;
}

void Logger::SaveTime(double time)
{
//    measureMap[m_threadCount].push_back(time);
}

void Logger::PrintTableHead(int vecSize, int loopCount)
{
    PrintLine();
    std::cout << "      " << "vector size = " << vecSize << "; "
            << "count of loop = " << loopCount << std::endl;
    PrintLine();
}

void Logger::PrintTimeTable()
{
    // TODO: change data structure for supporting new log format;
//    PrintTableHead();
    GetValues();

    std::cout << std::endl << "Impl type" << "          "
            << "Min        Max        Avg" << std::endl << std::endl;
    std::for_each(m_mapRes.begin(), m_mapRes.end(),
        std::bind2nd((std::ptr_fun(PrintTotalTime)), m_mapRes[PosOfStlMeasure].avg)
        );
}

void PrintTotalTime(std::pair<int, Values> pair, double stlAvg)
{
    switch (pair.first)
    {
        case Logger::PosOfStlMeasure: std::cout << "STL impl" << "           ";
            break;
        default:
            std::cout << pair.first << " threads impl" << "     ";

    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << pair.second.min << "s     " << pair.second.max << "s     "
              << pair.second.avg << "s ";

    if (!pair.first) std::cout << "STL" << std::endl;
    else if (pair.second.avg > stlAvg && stlAvg)
    {
        std::cout << "-" <<
            static_cast<int> ((pair.second.avg - stlAvg) / stlAvg * 100)
            << "%" << std::endl;
    }
    else if (pair.second.avg <= stlAvg && stlAvg)
    {
        std::cout << "+" <<
            static_cast<int> ((stlAvg - pair.second.avg) / pair.second.avg * 100)
            << "%" << std::endl;
    }
}
