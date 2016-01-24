#include "profiler.h"

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
