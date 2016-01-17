#ifndef POOL_H
#define POOL_H

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <fstream>

using std::cout;
using std::endl;

struct Coords //структура используется для передачи индексов                        
{ //сортируемого диапазона в очередь,
    int start; // start и end - крайние элементы диапазона
    int end; //который делитса индексом i 
    int i;
};

class Manager //класс содержит сортируемый вектор и методы для работы с ним
{
public:
    Manager(int threadsTotal); //конструктор принимает значение threadsTotal,
    //которое обозначает количество потоков, с которым будет работать класс
    // 0 - STL, положительные инт - количество потоков    
    void addWork(int vecSize); //метод принимает длину сортируемого вектора
    //и вносит индексы (начало 0 и конец vecSize) в очередь для обработки    
    void initVec(int vecSize); //заполняет вектор случайными значениями
    void initVecHalf(int vecSize); //заполняет первую половину вектора
    //значениями от 0 до vecSize/(1/2) последовательными значенями, вторую
    //половину вектора - случайными
    void printVec(int vecSize); //печатает элементы вектора от 0 до vecSize
    bool isJobEnd(); //метод проверят выставлен ли флаг обозначающий, что работа
    //завершена
    void jobIsEnd(); //метод выставляет флаг "работа завершена"
    bool isOtherTreadsWait(); //возвращает true, если количество ожидающих потоков
    //равно количество потоков
    //    void threadFree(); //увеличивает счётчик количества своб потоков на 1
    //    void threadBusy(); //уменьшает счётчик количества своб потоков на 1
    void exitThread(); //завершает поток, освобождая мутекс
    void reSet(int threadCount); //сбрасывает значения членов данных на значения
    //по-умолчанию, принимает количество потоков, с которомы потребуется работать

    std::vector<int> vec; //содержит сортируемый вектор int
    std::vector<std::string> vecStr; //используется при работе с текстовым файлами
    std::stack<Coords> globalDeq; //очередь, в которой сохраняются индексы участков
    //вектора, требующих сортировки

    volatile int waitOnCond_; //количество потоков локальная очередь которых
    //исчерпана, ожидающих появления "работы" в глобальной очереди
    volatile bool jobEnd_; //сигнализирует о том, что работа завершена -
    //выставляется потоком, завершающим работу последним
    int threadsTotal_; //общее количество потоков, с которыми должен работать класс

    static pthread_mutex_t mutexQue_; //мутекс, синхронизирующий доступ к очереди
    static pthread_cond_t cond; //переменная условия, на которой ожидают потоки,
    //в случае, если очередь пуста

private:
    Manager(const Manager& rhs); //копирование объектов запрещено
    Manager& operator=(const Manager& rhs); //присваивание объектов запрещено
};

struct SortingTask //структура, которая передаётся в поток, при его создании
{ //содержит номер потока и ссылку на класс, в котором содержится
    int id; //сортируемый вектор и методы работы с ним     
    Manager *mn;

    enum dType //перечисления определяет, с каким вектором (числовым или 
    { //строковым) будет работать поток (от этого зависит применяе-
        integer, string //мый потоком алгоритм сортировки
    } dataType;
};

class Lock //для работы с мутексом
{
    pthread_mutex_t & m_;
public:

    explicit Lock(pthread_mutex_t & m) : m_(m)
    {
        pthread_mutex_lock(&m_);
    }

    ~Lock()
    {
        pthread_mutex_unlock(&m_);
    }
};

void * f(void * arg); //функция потока

#endif  /* POOL_H */
