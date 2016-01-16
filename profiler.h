#ifndef PROFILER_H
#define PROFILER_H
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

///////////profiler
using namespace boost::posix_time;
using std::cout;
using std::endl;

class Prof
{
public:
    Prof();//конструктор вызывает StartMeas

    void StartMeas(); //сохраняет время, которое была вызвана функция в переменную
//  t1_  
    void StopAndPrint();//последовательно выполняет StopMeas и PrintMeas
    double StopAndGet();//вызывает StopMeas и возвращает значение разницы между
//    т1 и т2 для последующего использования логгером

private:

    ptime t1_; //сохраняет текущее значение времени
    ptime t2_; //сохраняет текущее значение времени
    void StopMeas();//сохраняет время, которое была вызвана функция в переменную
//   t2_ 
    void PrintMeas();//печатает разницу между т1 и т2 (в миллисекундах) в консоль

    Prof(const Prof& rhs); //копирование запрещено
    Prof& operator=(const Prof& rhs); //присваивание запрещено
};

///////////logger

enum stringType //обозначает строка с каким содержимым должна быть вызывана 
{               //при печате сводной таблицы
    pCqsort ,pStl, pOnethread, pTwothreads, pFourthreads
};

class Logger
{
public:
    Logger() : minValue_(0) {}
    void GetTime(stringType type, double);//принимает пару ТипИзмерения, значение
    //для сохранения в векторе результатов
    void PrintTime();//вызывает GetMinValue, после чего выводит в консоль 
    //таблицу результатов

    std::vector<std::pair<stringType, double> > vec;//содержит пары значений
    //ТипИзмерения, Значение

private:
    double minValue_;//используется для хранения минимального времени
//которое вычисляется среди всех значений вектора
    void GetMinValue();//вычисляет среди всех значений вектора минимальное
//    void PrintTotalTime(const std::pair<stringType,
//            double> pair);

    Logger(const Logger& rhs); //копирование запрещено
    Logger& operator=(const Logger& rhs); //присваивание запрещено
};

enum FillType //определяет способ заполнения вектора. fullSort - полное запол-
{               //нение случаными зачениями
    fullSort, halfSort //halfSort 1я половина отсортирована, вторая заполнена
};              //случайными значениями

#endif  /* PROFILER_H */
