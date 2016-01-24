#ifndef LOGGER_H
#define LOGGER_H

#include <map>

class Logger
{
public:
    typedef std::string SortName;
    typedef size_t ThreadsCount;
    typedef size_t ElementsCount;
    typedef size_t IterationCount;
    typedef double ExecutionTime;
    typedef std::vector<ExecutionTime> ExecutionTimeSet;

public:
    Logger();
    void SaveTime(double executionTime);
    void PrintTimeTable(const std::string& pivotSortName);
    void Reset();

    std::string sortName() const;
    void setSortName(const std::string& sortName);

    size_t elementsCount() const;
    void setElementsCount(const size_t& elementsCount);

    size_t threadCount() const;
    void setThreadCount(size_t threadCount);

private:
    Logger(const Logger& rhs); //копирование запрещено
    Logger& operator=(const Logger& rhs); //присваивание запрещено

private:
    // MAP sort name ->
    //      MAP count of elements ->
    //          PAIR threads count -> [iteration index, time diff] ENDPAIR
    //      ENDMAP
    // ENDMAP

    std::map<SortName, std::map<ElementsCount,
            std::map<ThreadsCount, ExecutionTimeSet > > > m_logMap;

    std::string m_sortName;
    size_t m_elementsCount;
    size_t m_threadsCount;
};

#endif // LOGGER_H
