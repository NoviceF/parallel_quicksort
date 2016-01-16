

#ifndef POOL_H
#define POOL_H

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <fstream>
#include <algorithm>

using std::cout;
using std::endl;

struct Coords   //структура используется для передачи индексов                        
{               //сортируемого диапазона в очередь,
    int start;  // start и end - крайние элементы диапазона
    int end;    //который делитса на 2 части 
    int i;      //индексом i (start,(i-1)), ((i+1),end)
};

class Manager   //класс содержит сортируемый вектор и методы для работы с ним
{
public:
    Manager(){};
//    Manager(int threadsTotal);//конструктор принимает значение threadsTotal,
    //которое обозначает количество потоков, с которым будет работать класс    
//    void addWork(int vecSize); //метод принимает длину сортируемого вектора
    //и вносит индексы (начало 0 и конец vecSize) в очередь для обработки    
    void initVec(int vecSize); //заполняет вектор случайными значениями
    void initVecHalf(int vecSize);//заполняет первую половину вектора
    //значениями от 0 до vecSize/(1/2) последовательными значенями, вторую
    //половину вектора - случайными
    void printVec(int vecSize); //печатает элементы вектора от 0 до vecSize
//    bool isJobEnd(); //метод проверят выставлен ли флаг обозначающий, что работа
    //завершена
//    void jobIsEnd();//метод выставляет флаг "работа завершена"
//    bool isAllTreadsWait();//возвращает true, если количество ожидающих потоков
    //равно количество потоков
//    void threadFree();//увеличивает счётчик количества своб потоков на 1
//    void threadBusy();//уменьшает счётчик количества своб потоков на 1
//    void exitThread();//когда isJobEnd == true метод завершает поток, 
    //освобождая мутекс
//    void reSet(int threadCount);//сбрасывает значения членов данных на значения
    //по-умолчанию, принимает количество потоков, с которомы потребуется работать

    std::vector<int> vec;//используется, при работе с int
    std::vector< std::vector<int> > vecArr;
//    std::vector<std::string> vecStr;//используется при работе с текстовым файлами
//    std::deque<Coords> coordsDeq;//очередь, в которой сохраняются индексы участков
    //вектора, требующие сортировки

//    volatile int threadFree_;//количество свободных потоков
//    volatile bool jobEnd_;//сигнализирует о том, что работа завершена - вектор
    //отсортирован
//    int threadsTotal_;//общее количество потоков, с которыми должен работать класс

//    static pthread_mutex_t mutexQue_; //мутекс, синхронизирующий доступ к очереди

//    static pthread_cond_t cond;//переменная условия, на которой ожидают потоки,
    //в случае, если очередь пуста

private:
    Manager(const Manager& rhs); //копирование объектов запрещено
    Manager& operator=(const Manager& rhs);//присваивание объектов запрещено
};

struct SortingTask //структура, которая передаётся в поток, при его создании
{                  //содержит номер потока и ссылку на класс, в котором содержится
    int id;        //сортируемый вектор и методы работы с ним     
    Manager *mn;

    enum dType     //перечисления определяет, с каким вектором (числовым или 
    {             //строковым) будет работать поток (от этого зависит применяе-
        integer, string //мый потоком алгоритм сортировки
    } dataType;
};

struct VecToMerge
{
    int id;
    std::vector<int>* temp1;
    std::vector<int>* temp2;
    std::vector<int> result;
};

void * f(void * arg); //функция потока
void * fmerge(void * arg);
#endif  /* POOL_H */
