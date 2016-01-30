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
    void saveTime(double executionTime);
    void printTimeTable(const std::string& pivotSortName);
    void reset();

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


//    void VectorSaver::SaveResToFile(std::vector<int>& vec, int thrCount, int mesNum)
//    {
//        std::string str1("./test/");
//        mkdir(str1.c_str(), 0777);
//        str1.append("_thrCount");
//        str1.append(::lexical_cast<std::string, int>(thrCount));
//        str1.append("_mesNum");
//        str1.append(::lexical_cast<std::string, int>(mesNum));
//        std::ofstream output(str1, std::ios::binary);

//        if (output.is_open())
//        {
//            std::copy(vec.begin(), vec.end(),
//                      std::ostream_iterator<int>(output, "\n"));
//            output.close();
//        }
//    }


#endif // LOGGER_H
