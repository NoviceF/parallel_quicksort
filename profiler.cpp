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

double Prof::StopAndGet()
{
    StopMeas();
    if (t1_ < t2_)
    {
        time_duration td(t2_ - t1_);
        double temp(td.total_milliseconds());
        return temp / 1000;
    } else cout << "Can't measure time!" << endl;
    return -1;

}

void Prof::StartMeas()
{
    t1_ = microsec_clock::local_time();
}

void Prof::StopMeas()
{
    t2_ = microsec_clock::local_time();
}

void Prof::PrintMeas()
{
    if (t1_ < t2_)
    {
        time_duration td(t2_ - t1_);
        double temp(td.total_milliseconds());
        cout << endl << temp / 1000 << " seconds elapsed\n" << endl;
    } else cout << "Can't measure time!" << endl;
}


////////////////
//logger impl//
///////////////

double GetMinValue(std::vector<double> vec);
double GetMaxValue(std::vector<double> vec);
double GetAvgValue(std::vector<double> vec);

void PrintLine(int lengh = 54)
{
    for (int i = 0; i < lengh; ++i)
    {
        cout << "-";
    }
    cout << endl;
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
        mapRes[it->first].min = GetMinValue(it->second);
        mapRes[it->first].max = GetMaxValue(it->second);
        mapRes[it->first].avg = GetAvgValue(it->second);
    }
}

void Logger::GetTime(int threadCount, double val)
{
    map[threadCount].push_back(val);
}

void Logger::PrintTime(int vecSize, int loopCount)
{
    GetValues();
    PrintLine();
    cout << "      " << "vector size = " << vecSize << "; "
            << "count of loop = " << loopCount << endl;
    PrintLine();

    cout << endl << "Impl type" << "          "
            << "Min        Max        Avg" << endl << endl;
    std::for_each(mapRes.begin(), mapRes.end(),
            std::bind2nd((std::ptr_fun(PrintTotalTime)), mapRes[0].avg)
            );

}

void PrintTotalTime(std::pair<int, Values> pair, double stlAvg)
{
    switch (pair.first)
    {
        case 0: cout << "STL impl" << "           ";
            break;
        default:
            cout << pair.first << " threads impl" << "     ";

    }
    cout << std::fixed << std::setprecision(3);
    cout << pair.second.min << "s     " << pair.second.max << "s     "
            << pair.second.avg << "s ";

    if (!pair.first) cout << "STL" << endl;
    else if (pair.second.avg > stlAvg && stlAvg) cout << "-" <<
            static_cast<int> ((pair.second.avg - stlAvg) / stlAvg * 100)
            << "%" << endl;
    else if (pair.second.avg <= stlAvg && stlAvg)cout << "+" <<
            static_cast<int> ((stlAvg - pair.second.avg) / pair.second.avg * 100)
            << "%" << endl;
}

double GetMinValue(std::vector<double> vec)
{
    if (vec.empty())
    {
        cout << "vector is empty!" << endl;
        return -1;
    }
    double minValue = vec[0];
    for (int i = 0; i < static_cast<int> (vec.size()); ++i)
    {
        if (vec[i] < minValue) minValue = vec[i];
    }
    return minValue;
}

double GetMaxValue(std::vector<double> vec)
{
    if (vec.empty())
    {
        cout << "vector is empty!" << endl;
        return -1;
    }
    double maxValue = vec[0];
    for (int i = 0; i < static_cast<int> (vec.size()); ++i)
    {
        if (vec[i] > maxValue) maxValue = vec[i];
    }
    return maxValue;
}

double GetAvgValue(std::vector<double> vec)
{
    if (vec.empty())
    {
        cout << "vector is empty!" << endl;
        return -1;
    }
    double avgValue = 0;

    for (int i = 0; i < static_cast<int> (vec.size()); ++i)
    {
        avgValue += vec[i];
    }
    avgValue /= static_cast<int> (vec.size());
    return avgValue;
}

