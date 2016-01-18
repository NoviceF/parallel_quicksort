#include <algorithm>
#include <cassert>

#include "profiler.h"

/////////////////
//profiler impl//
/////////////////

Prof::Prof() /*: measValue_(0)*/
{
    StartMeas();
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

void Logger::Reset()
{
    map.clear();
    mapRes.clear();
}

void Logger::GetValues()
{
    for (std::map<int, std::vector<double> >::iterator it = map.begin();
            it != map.end(); ++it)
    {
        assert(!it->second.empty());

        MinMaxAvg minMaxAvg = GetMinMaxAvg(it->second);
        mapRes[it->first].min = minMaxAvg.min;
        mapRes[it->first].max = minMaxAvg.max;
        mapRes[it->first].avg = minMaxAvg.avg;
    }
}

void Logger::SaveTime(int threadCount, double time)
{
    map[threadCount].push_back(time);
}

void Logger::PrintTimeTable(int vecSize, int loopCount)
{
    GetValues();
    PrintLine();
    std::cout << "      " << "vector size = " << vecSize << "; "
            << "count of loop = " << loopCount << std::endl;
    PrintLine();

    std::cout << std::endl << "Impl type" << "          "
            << "Min        Max        Avg" << std::endl << std::endl;
    std::for_each(mapRes.begin(), mapRes.end(),
        std::bind2nd((std::ptr_fun(PrintTotalTime)), mapRes[PosOfStlMeasure].avg)
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
