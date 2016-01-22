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

class Logger
{
public:
    static const int PosOfStlMeasure = 0;

    Logger();
    void SaveTime(double executionTime);
//    void PrintTableHead(int vecSize, int loopCount = 1);
    void PrintTimeTable();
    void Reset();

    std::string sortName() const;
    void setSortName(const std::string& sortName);

    size_t elementsCount() const;
    void setElementsCount(const size_t& elementsCount);

    size_t threadCount() const;
    void setThreadCount(size_t threadCount);

    size_t iteartionIndex() const;
    void setIteartionIndex(size_t iteartionIndex);

private:
    //анализирует результаты всех проведённых тестов,
    //сохранённые в map, расчитывает мин, макс и среднее время
    void CalcValues();

    Logger(const Logger& rhs); //копирование запрещено
    Logger& operator=(const Logger& rhs); //присваивание запрещено

private:

    struct LogValue
    {
        size_t iterationIndex;
        double runningTime;
    };

    // MAP sort name ->
    //      MAP count of elements ->
    //          PAIR threads count -> [iteration index, time diff] ENDPAIR
    //      ENDMAP
    // ENDMAP

    typedef std::string SortName;
    typedef std::vector<LogValue> LogValueCollection;
    typedef size_t ThreadsCount;
    typedef size_t ElementsCount;

    std::map<SortName, std::map<ElementsCount,
            std::map<ThreadsCount, LogValueCollection > > > m_logMap;

    std::string m_sortName;
    size_t m_elementsCount;
    size_t m_threadsCount;
    size_t m_iterationIndex;
};
#endif  /* PROFILER_H */
