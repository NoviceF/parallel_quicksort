#ifndef PROFILER_H
#define PROFILER_H

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::gregorian;

///////////profiler
using namespace boost::posix_time;
using std::cout;
using std::endl;

class Prof
{
public:
    Prof(); //конструктор вызывает StartMeas

    void StartMeas(); //сохраняет время, в которое была вызвана функция в переменную
    //  t1_  
    void StopAndPrint(); //последовательно выполняет StopMeas и PrintMeas
    double StopAndGet(); //вызывает StopMeas и возвращает значение разницы между
    //    т1 и т2 для последующего использования логгером

private:

    ptime t1_; //сохраняет текущее значение времени
    ptime t2_; //сохраняет текущее значение времени
    void StopMeas(); //сохраняет время, которое была вызвана функция в переменную
    //   t2_ 
    void PrintMeas(); //печатает разницу между т1 и т2 (в миллисекундах) в консоль

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

    Logger() { }
    void GetTime(int threadCount, double val); //принимает пару кол-во потоков,
    //значение для сохранения в векторе результатов
    void PrintTime(int vecSize, int loopCount = 1);
    // печатает таблицу результатов в консоль
    void Reset(); //сбрасывает состояние логгера

    std::map<int, std::vector<double> > map; //содержит пары значений
    //количество потоков (0 - STL), время сортировки

private:
    void GetValues(); //анализирует результаты всех проведённых тестов,
    //сохранённые в map, расчитывает мин, макс и среднее время
    std::map<int, Values> mapRes; //содержит результаты измерений, в виде
    //пары - колво потоков, мин, макс среднее значение
    
    Logger(const Logger& rhs); //копирование запрещено
    Logger& operator=(const Logger& rhs); //присваивание запрещено
};
#endif  /* PROFILER_H */
