#include "manager.h"
#include "profiler.h"
//#include "progopimpl.h"

#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

using namespace std;

void doSTLsort(int vecSize, Logger& logger, FillType type);
void DoSort1(int vecSize, Logger& logger, FillType type);
void DoSort2(int vecSize, Logger& logger, FillType type);
void DoSort4(int vecSize, Logger& logger, FillType type);
void startAll(pthread_t *threads, int threadsCount, Manager &mn,
        SortingTask::dType type = SortingTask::integer);
void startAllMerge(pthread_t *threads, int threadsCount, VecToMerge *vecs);
//принимает указатель на массив переменных pthread_t, кол-во потоков, которые
//необходимо создать, тип данных в сортируемом вектора integer, либо string
void waitAll(pthread_t *threads, int threadsCount);
//ожидает завершения всех потоков
template <typename T>
void CheckSorted(std::vector<T> data, int size);
//проверяет отсортирован ли массив, выводит информацию в консоль
//void doSortInt(int threadsCount, int vecSize, Logger& logger, FillType type);
//содержит все необходимые действия для запуска сортировки целых чисел
//принимает Количество потоков, размер сортирумого вектора, ссылку на логер
//тип заполнения (см. FillType), который необходимо применить к вектору

//void doSTLsort(int vecSize, Logger& logger, FillType type);
//содержит все необходимые действия для запуска сортировки целых чисел при помощи
//STL, принимает размер вектора, ссылку на Логгер, тип заполнения вектора
//void doSortString(int threadsCount, Logger& logger, std::string path);
//содержит все необходимые действия для запуска сортировки строк, принимает
//количество потоков, которое следует использовать для сортировки, ссылку на логер
//путь к сортируемому файлу

bool IntLess(int i1, int i2) //предикат для алгоритма sort
{
    return i1<i2;
}

int IntLessCSTD(const void * a, const void * b)
{
    return ( *(int*) a - *(int*) b);
}

int main(int ac, char* av[])
{
    int threadsCount = 1; //количество потоков, которое следует использовать
    //    для сортировки
    int size = 1000000; //задаём требуемый размер вектора
    //    FillType fType = //halfSort;        
    //            fullSort;
    Manager mn;

    mn.initVec(size);

    FillType fType = //halfSort;        
            fullSort;

    vector<int> tempVec(mn.vec);

    Logger logger;

    {
        int* a = new int [size];
        std::copy(mn.vec.begin(), mn.vec.end(), a);
        Prof prof;
        qsort(a, size, sizeof (int), IntLessCSTD);
        logger.GetTime(pCqsort, prof.StopAndGet());
        mn.vec.assign(a, a + size);
        CheckSorted(mn.vec, mn.vec.size());
        delete [] a;
    }
    //    mn.printVec(10);

    doSTLsort(size, logger, fType);
    DoSort1(size, logger, fType);
    DoSort2(size, logger, fType);
    DoSort4(size, logger, fType);

    //    std::sort(tempVec.begin(), tempVec.end(), IntLess);
    //    logger.GetTime(pStl, prof.StopAndGet());


    logger.PrintTime();

    return 0;
}

template <typename T>
void CheckSorted(std::vector<T> data, int size)
{
    if (data.empty())
    {
        cout << "checking vector size = 0" << endl;
        return;
    }
    //    cout << "sequence size is " << size << endl;
    cout << "sequence is ";

    for (int i = 0; i < static_cast<int> (data.size()) - 1; ++i)
    {
        if (data[i] > data[i + 1])
        {
            cout << "not sorted!" << endl;
            return;
        }
    }
    cout << "sorted!" << endl;
    //    return true;
}

void waitAll(pthread_t *threads, int threadsCount)
{
    for (int i = 0; i < threadsCount; ++i)
    {
        pthread_join(threads[i], NULL);
    }
}

void startAll(pthread_t *threads, int threadsCount, Manager &mn,
        SortingTask::dType type)
{
    SortingTask* tasks = new SortingTask [threadsCount];

    for (int i = 0; i < threadsCount; ++i)
    {
        tasks[i].id = i;
        tasks[i].mn = &mn;
        tasks[i].dataType = type;

        int status = pthread_create(&threads[i], NULL, f, &tasks[i]);
        if (status != 0)
        {
            cout << "thread starting failed" << endl;
        }
    }

    waitAll(threads, threadsCount);

    delete []tasks;
}

void startAllMerge(pthread_t *threads, int threadsCount, VecToMerge* vecs)
{
    //    SortingTask* tasks = new SortingTask [threadsCount];

    for (int i = 0; i < threadsCount; ++i)
    {
        vecs[i].id = i;


        int status = pthread_create(&threads[i], NULL, fmerge, &vecs[i]);
        if (status != 0)
        {
            cout << "thread starting failed" << endl;
        }
    }

    waitAll(threads, threadsCount);

}

void doSTLsort(int vecSize, Logger& logger, FillType type)
{
    Manager mn1;
    if (type == fullSort) mn1.initVec(vecSize);
    else mn1.initVecHalf(vecSize);
    Prof prof;
    std::sort(mn1.vec.begin(), mn1.vec.end(), IntLess);
    logger.GetTime(pStl, prof.StopAndGet());
    CheckSorted(mn1.vec, mn1.vec.size());
}

void DoSort1(int vecSize, Logger& logger, FillType type)
{
    int threadsCount = 1;
    Manager mn1;
    if (type == fullSort) mn1.initVec(vecSize);
    else mn1.initVecHalf(vecSize);
    mn1.vecArr.push_back(vector<int>(mn1.vec.begin(), mn1.vec.end()));
    pthread_t *threads = new pthread_t[threadsCount];
    Prof prof;
    startAll(threads, threadsCount, mn1);
    mn1.vec.clear();
    mn1.vec.reserve(vecSize);
    mn1.vec.assign(mn1.vecArr[0].begin(), mn1.vecArr[0].end());
    logger.GetTime(pOnethread, prof.StopAndGet());

    delete [] threads;
    CheckSorted(mn1.vec, mn1.vec.size());
}

void DoSort2(int vecSize, Logger& logger, FillType type)
{
    int threadsCount = 2;
    Manager mn1;
    if (type == fullSort) mn1.initVec(vecSize);
    else mn1.initVecHalf(vecSize);

    int i = vecSize / 2;
    mn1.vecArr.push_back(vector<int>(mn1.vec.begin(), mn1.vec.begin() + i));
    mn1.vecArr.push_back(vector<int>(mn1.vec.begin() + i, mn1.vec.end()));

    pthread_t *threads = new pthread_t[threadsCount];
    Prof prof;
    startAll(threads, threadsCount, mn1);
    mn1.vec.clear();
    mn1.vec.reserve(vecSize);

    merge(mn1.vecArr[0].begin(), mn1.vecArr[0].end(),
            mn1.vecArr[1].begin(), mn1.vecArr[1].end(),
            back_inserter(mn1.vec));
    logger.GetTime(pTwothreads, prof.StopAndGet());

    delete [] threads;
    CheckSorted(mn1.vec, mn1.vec.size());
}

void DoSort4(int vecSize, Logger& logger, FillType type)
{
    int threadsCount = 4;
    Manager mn1;
    if (type == fullSort) mn1.initVec(vecSize);
    else mn1.initVecHalf(vecSize);

    int i = vecSize / 2;
    int j = i / 2;

    mn1.vecArr.push_back(vector<int>(mn1.vec.begin(),
            mn1.vec.begin() + j));
    mn1.vecArr.push_back(vector<int>(mn1.vec.begin() + j,
            mn1.vec.begin() + i));
    mn1.vecArr.push_back(vector<int>(mn1.vec.begin() + i,
            mn1.vec.begin() + (i + j)));
    mn1.vecArr.push_back(vector<int>(mn1.vec.begin() + (i + j),
            mn1.vec.end()));

    pthread_t *threads = new pthread_t[threadsCount];
    Prof prof;
    startAll(threads, threadsCount, mn1);
    mn1.vec.clear();
    mn1.vec.reserve(vecSize);

    delete [] threads;

    threadsCount = 2;
    threads = new pthread_t[threadsCount];
    VecToMerge* vecs = new VecToMerge [threadsCount];

    //    for (int i = 0; i < threadsCount; ++i)
    //    {
    //        vecs[i].temp1 = &mn1.vecArr[i];
    //    }
    vecs[0].temp1 = &mn1.vecArr[0];
    vecs[0].temp2 = &mn1.vecArr[1];
    vecs[1].temp1 = &mn1.vecArr[2];
    vecs[1].temp2 = &mn1.vecArr[3];
    startAllMerge(threads, threadsCount, vecs);
    //    merge(vecs[0].result.begin(), vecs[0].result.end(),
    //            vecs[1].result.begin(), vecs[1].result.end(),
    //            back_inserter(mn.vec));

    std::vector<int> temp1;
    std::vector<int> temp2;
    merge(mn1.vecArr[0].begin(), mn1.vecArr[0].end(),
            mn1.vecArr[1].begin(), mn1.vecArr[1].end(), back_inserter(temp1));
    merge(mn1.vecArr[2].begin(), mn1.vecArr[2].end(),
            mn1.vecArr[3].begin(), mn1.vecArr[3].end(), back_inserter(temp2));

    merge(temp1.begin(), temp1.end(),
            temp2.begin(), temp2.end(),
            back_inserter(mn1.vec));
    logger.GetTime(pFourthreads, prof.StopAndGet());

    delete [] threads;
    CheckSorted(mn1.vec, mn1.vec.size());
}








/////////////////////////////////////////



//void doSortInt(int threadsCount, int vecSize, Logger& logger, FillType type)
//{
//    stringType stringImpl;
//
//    switch (threadsCount)
//    {
//        case 1: stringImpl = pOnethread;
//            break;
//        case 2: stringImpl = pTwothreads;
//            break;
//        case 4: stringImpl = pFourthreads;
//            break;
//        default:
//        {
//            cout << "Unknown count of threads!";
//            return;
//        }
//    }


//    Manager mn1(threadsCount);
//    //    cout << "vecsize = " << vecSize << endl;
//    if (type == fullSort) mn1.initVec(vecSize);
//    else mn1.initVecHalf(vecSize);
//    mn1.addWork(vecSize);
//
//    pthread_t *threads = new pthread_t[threadsCount];
//
//    Prof prof;
//    startAll(threads, threadsCount, mn1);
//    logger.GetTime(stringImpl, prof.StopAndGet());
//
//    CheckSorted(mn1.vec, mn1.vec.size());
//
//
//    delete []threads;
//}
//
//void doSortString(int threadsCount, Logger& logger, std::string path)
//{
//    stringType stringImpl;
//
//    switch (threadsCount)
//    {
//        case 1: stringImpl = pOnethread;
//            break;
//        case 2: stringImpl = pTwothreads;
//            break;
//        case 4: stringImpl = pFourthreads;
//            break;
//        default:
//        {
//            cout << "Unknown count of threads!";
//            return;
//        }
//    }
//    //"COPYING"
//    std::ifstream inputFile(path);
//    std::string str;
//    Manager mn1(threadsCount);
//
//    while (inputFile >> str) mn1.vecStr.push_back(str);
//
//    mn1.addWork(mn1.vecStr.size());
//
//    //    cout << "vecsize = " << vecSize << endl;
//
//    pthread_t *threads = new pthread_t[threadsCount];
//
//    Prof prof;
//    startAll(threads, threadsCount, mn1, SortingTask::string);
//    logger.GetTime(stringImpl, prof.StopAndGet());
//
//    CheckSorted(mn1.vecStr, mn1.vecStr.size());
//    //    for (int i = 0; i < mn1.vecStr.size(); ++i)
//    //    {
//    //        cout << mn1.vecStr[i] << " ";
//    //    }
//
//    delete []threads;
//}

