#include <algorithm>
#include <functional>

#include <sys/stat.h>

#include "manager.h"
#include "profiler.h"
#include "commandpromptparser.h"

void startAll(pthread_t *threads, int threadsCount, Manager &mn,
              SortingTask::dType type = SortingTask::integer);
//принимает указатель на массив переменных pthread_t, кол-во потоков, которые
//необходимо создать, тип данных в сортируемом вектора integer, либо string
void waitAll(pthread_t *threads, int threadsCount);
//ожидает завершения всех потоков
bool IntLess(int i1, int i2) //предикат для алгоритма sort
{
    return i1<i2;
}
template <typename T>
void CheckSorted(std::vector<T> data, int size);
//проверяет отсортирован ли массив, если нет - выводит информацию в консоль
void doSortInt(std::vector<int> vec, int threadsCount, Logger& logger,
               std::vector<std::pair<std::vector<int>, int> >& globalVec,
               FillType type);
//содержит все необходимые действия для запуска сортировки целых чисел
//принимает Количество потоков, размер сортирумого вектора, ссылку на логер
//тип заполнения (см. FillType), который необходимо применить к вектору
void doSTLsort(std::vector<int> vec, Logger& logger,
               std::vector<std::pair<std::vector<int>, int> >& globalVec,
               FillType type);
//содержит все необходимые действия для запуска сортировки целых чисел при помощи
//STL, принимает размер вектора, ссылку на Логгер, тип заполнения вектора
void doSortString(int threadsCount, Logger& logger, std::string path);
//содержит все необходимые действия для запуска сортировки строк, принимает
//количество потоков, которое следует использовать для сортировки, ссылку на логер
//путь к сортируемому файлу
//void SaveResToFile(std::vector<int>& vec, int threadsCount, int mesNum);
////сохраняет содержимое вектора в в файлы

struct VectorSaver
{
    std::vector< std::pair<std::vector<int>, int> > vec;
    void SaveResToFile(std::vector<int>& vecInt,
            int thrCount, int mesNum);
};

int main(int argc, char* argv[])
{
    VectorSaver vecSaver;

    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());

    Manager mn1(params.threadsCount);
    mn1.initVec(params.firstVecSize);
    //    mn1.addWork(vecSize);
    Logger logger;

    size_t vecSize = params.firstVecSize;
    size_t thrCountInTest = params.thrCountInTest;

    do
    {
        logger.Reset();

        for (size_t i = 0; i < params.loopCount; ++i)
        {
            doSTLsort(mn1.vec, logger, vecSaver.vec, params.type);

            if (thrCountInTest)
            {
                int temp = thrCountInTest;
                for (size_t i = 2; i < thrCountInTest + 1;
                     thrCountInTest /= 2)
                {
                    doSortInt(mn1.vec, thrCountInTest, logger,
                            vecSaver.vec, params.type);
                }
                thrCountInTest = temp;
            }
            else
            {
                doSortInt(mn1.vec, params.threadsCount, logger,
                          vecSaver.vec, params.type);
            }
        }

        logger.PrintTimeTable(vecSize, params.loopCount);
        vecSize += params.incStep;

        mn1.vec.clear();
        mn1.initVec(vecSize);
    }
    while (vecSize < params.lastVecSize + 1);

    std::cout << std::endl;

    for (int i = 0; i < static_cast<int> (vecSaver.vec.size()); ++i)
    {
        vecSaver.SaveResToFile(vecSaver.vec[i].first, vecSaver.vec[i].second, i);
    }

    return 0;
}

void doSTLsort(std::vector<int> vec, Logger& logger,
        std::vector<std::pair<std::vector<int>, int> >& globalVec, FillType type)
{
    int nothread = 0;
    Manager mn1(nothread);

    if (type == fullSort)
        mn1.initVec(vec.size());
    else
        mn1.initVecHalf(vec.size());

    mn1.vec = vec;
    mn1.addWork(vec.size());

    Prof prof;
    std::sort(mn1.vec.begin(), mn1.vec.end() , IntLess);
    logger.SaveTime(Logger::PosOfStlMeasure, prof.StopAndGetDifference());

    CheckSorted(mn1.vec, mn1.vec.size());
    globalVec.push_back(make_pair(mn1.vec, nothread));
}

void doSortInt(std::vector<int> vec, int threadsCount, Logger& logger,
        std::vector<std::pair<std::vector<int>, int> >& globalVec, FillType type)
{
    //    cout << "vecsize in INTsort = " << vec.size() << endl;
    Manager mn1(threadsCount);
    //    cout << "vecsize = " << vecSize << endl;

    if (type == fullSort)
        mn1.initVec(vec.size());
    else
        mn1.initVecHalf(vec.size());

    mn1.vec = vec;
    mn1.addWork(vec.size());

    pthread_t *threads = new pthread_t[threadsCount];

    Prof prof;
    startAll(threads, threadsCount, mn1);
    logger.SaveTime(threadsCount, prof.StopAndGetDifference());
    globalVec.push_back(make_pair(mn1.vec, threadsCount));
    CheckSorted(mn1.vec, mn1.vec.size());

    delete []threads;
}

template <typename T>
void CheckSorted(std::vector<T> data, int)
{
    if (data.empty())
    {
        cout << "checking vector size = 0" << endl;
        return;
    }
    for (int i = 0; i < static_cast<int> (data.size()) - 1; ++i)
    {
        if (data[i] > data[i + 1])
        {
            cout << "sequence is not sorted!" << endl;
            return;
        }
    }
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

void VectorSaver::SaveResToFile(std::vector<int>& vec, int thrCount, int mesNum)
{
    std::string str1("./test/");
    mkdir(str1.c_str(), 0777);
    str1.append("_thrCount");
    str1.append(::lexical_cast<std::string, int>(thrCount));
    str1.append("_mesNum");
    str1.append(::lexical_cast<std::string, int>(mesNum));
    std::ofstream output(str1, std::ios::binary);

    if (output.is_open())
    {
        std::copy(vec.begin(), vec.end(),
                  std::ostream_iterator<int>(output, "\n"));
        output.close();
    }
}

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


