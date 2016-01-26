#ifndef PROFILER_H
#define PROFILER_H

#include <posix_time.hpp>

class Prof
{
public:
    Prof();

    void StartMeas();
    void StopAndPrint();
    double StopAndGetDifference();

private:
    boost::posix_time::ptime t1_;
    boost::posix_time::ptime t2_;
    void StopMeas();
    //печатает разницу между т1 и т2 (в миллисекундах) в консоль
    void PrintMeas() const;

    Prof(const Prof& rhs); //копирование запрещено
    Prof& operator=(const Prof& rhs); //присваивание запрещено
};

#endif  /* PROFILER_H */
