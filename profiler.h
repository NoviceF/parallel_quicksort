#ifndef PROFILER_H
#define PROFILER_H

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

///////////profiler

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

///////////logger

struct Values //структура для хранения значений времени
{
    double min;
    double max;
    double avg;
};

class Logger
{
public:
    static const int PosOfStlMeasure = 0;

    Logger();
    void SaveTime(double time);
    void PrintTableHead(int vecSize, int loopCount = 1);
    void PrintTimeTable();
    void Reset();

    const std::map<int, std::vector<double> >& measureMap() const;

    int threadCount() const;
    void setThreadCount(int threadCount);

    int iteartionCount() const;
    void setIteartionCount(int iteartionCount);

private:
    //анализирует результаты всех проведённых тестов,
    //сохранённые в map, расчитывает мин, макс и среднее время
    void GetValues();

    Logger(const Logger& rhs); //копирование запрещено
    Logger& operator=(const Logger& rhs); //присваивание запрещено

private:
    //содержит пары значений
    //количество потоков (0 - STL), время сортировки
    std::map<int, std::vector<double> > m_map;

    //содержит результаты измерений, в виде
    //пары - колво потоков, мин, макс среднее значение
    std::map<int, Values> m_mapRes;
    
    int m_threadCount;
    int m_iteartionCount;
};
#endif  /* PROFILER_H */
