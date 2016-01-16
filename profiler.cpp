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


/////////////////
//logger impl//
/////////////////

void PrintTotalTime(const std::pair<stringType,
        double> pair, double minValue);

void Logger::GetTime(stringType type, double val)
{
    vec.push_back(std::make_pair(type, val));
}

void Logger::PrintTime()
{
    GetMinValue();
    cout << endl << "Impl type" << "          "
            << "elapsed time   difference" << endl << endl;

    std::for_each(vec.begin(), vec.end(), std::bind2nd(
            std::ptr_fun(PrintTotalTime), minValue_)
            );

}

void Logger::GetMinValue()
{
    if (vec.empty())
    {
        cout << "vector is empty!" << endl;
        return;
    }
    minValue_ = vec[0].second;
    for (int i = 0; i < static_cast<int> (vec.size()); ++i)
    {
        if (vec[i].second < minValue_) minValue_ = vec[i].second;
    }
}

void PrintTotalTime(const std::pair<stringType,
        double> pair, double minValue)
{
    switch (pair.first)
    {
        case pCqsort: cout << "Cqsort impl" << "        ";
            break;
        case pStl: cout << "STL impl" << "           ";
            break;
        case pOnethread: cout << "Onethread impl" << "     ";
            break;
        case pTwothreads: cout << "Twothreads impl" << "    ";
            break;
        case pFourthreads: cout << "Fourthreads impl" << "   ";
            break;
    }
    cout << std::fixed << std::setprecision(3);
    cout << pair.second << "s         ";
    if (minValue < pair.second) cout << "+" << pair.second - minValue
            << "s" << " ("
            << static_cast<int>((pair.second - minValue)/minValue*100)
            << "%)" << endl;

    else cout << "it's top time!" << endl;

}
