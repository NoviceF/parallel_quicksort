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

// структура используется для передачи индексов
// сортируемого диапазона в очередь
struct Coords
{
    size_t start;
    size_t end;
    size_t pivot;
};

// класс содержит сортируемый вектор и методы для работы с ним
class Manager
{
public:
    // конструктор принимает значение threadsTotal,
    // которое обозначает количество потоков, с которым будет работать класс
    //  0 - STL, положительные инт - количество потоков
    Manager(std::vector<int>& sourceVec, int threadsTotal);
    // метод принимает длину сортируемого вектора
    // и вносит индексы (начало 0 и конец vecSize) в очередь для обработки
    void addWork(size_t vecSize);

    bool isJobDone();
    void jobIsDone();

    // возвращает true, если количество ожидающих потоков равно количеству потоков
    bool isOtherTreadsWait();
    // завершает поток, освобождая мьютекс
    void exitThread();
    // сбрасывает значения членов данных на значения
    // по-умолчанию, принимает количество потоков, с которомы потребуется работать
    void reset(int threadCount);

    std::vector<int>& vec();

    std::stack<Coords>& globalDeq();

    volatile int& waitOnCond();

    volatile bool& jobEnd();

    static pthread_mutex_t mutexQue; // мьютекс, синхронизирующий доступ к очереди
    // переменная условия, на которой ожидают потоки,
    // в случае, если очередь пуста
    static pthread_cond_t cond;

private:
    std::vector<int>& vec_; // содержит сортируемый вектор int
    std::vector<std::string> vecStr_; // используется при работе с текстовым файлами
    // очередь, в которой сохраняются индексы участков
    // вектора, требующих сортировки
    std::stack<Coords> globalDeq_;

    // количество потоков локальная очередь которых
    // исчерпана, ожидающих появления "работы" в глобальной очереди
    volatile int waitOnCond_;
    // сигнализирует о том, что работа завершена -
    // выставляется потоком, завершающим работу последним
    volatile bool jobEnd_;
    int threadsTotal_; // общее количество потоков, с которыми должен работать класс

private:
    Manager(const Manager& rhs); // копирование объектов запрещено
    Manager& operator=(const Manager& rhs); // присваивание объектов запрещено
};

// структура, которая передаётся в поток, при его создании
// содержит номер потока и ссылку на класс, в котором содержится
// сортируемый вектор и методы работы с ним
struct SortingTask
{
    int id;
    Manager* mn;

    // перечисления определяет, с каким вектором (числовым или
    // строковым) будет работать поток (от этого зависит применяе-
    // мый потоком алгоритм сортировки
    enum dType
    {
        integer, string
    } dataType;
};

// для работы с мьютексом
class Lock
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

void * f(void * arg); // функция потока

#endif  /* POOL_H */
