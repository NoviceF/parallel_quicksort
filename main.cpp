#include <algorithm>
#include <functional>

#include <sys/stat.h>

#include "commandpromptparser.h"
#include "sort.h"
#include "sorttest.h"

void startAll(pthread_t *threads, int threadsCount, Manager &mn,
              SortingTask::dType type = SortingTask::integer);
//принимает указатель на массив переменных pthread_t, кол-во потоков, которые
//необходимо создать, тип данных в сортируемом вектора integer, либо string
void waitAll(pthread_t *threads, int threadsCount);
//ожидает завершения всех потоков
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

//struct VectorSaver
//{
//    std::vector< std::pair<std::vector<int>, int> > vec;
//    void SaveResToFile(std::vector<int>& vecInt,
//            int thrCount, int mesNum);
//};



int main(int argc, char* argv[])
{
    CommandPromptParser promptParser(argc, argv);
    const ParsedParams params(promptParser.getParams());
    Logger logger;

    typedef CSort<int> csortInt;
    SingleThreadSortTest<csortInt> csortTest(params);
    csortTest.runTest(logger);

    typedef STLSort<int> stlSortInt;
    SingleThreadSortTest<stlSortInt> stlSortTest(params);
    stlSortTest.runTest(logger);

    typedef PosixParallelSort<int> posixSortInt;
    MultiThreadSortTest<posixSortInt> posixSortTest(params);
    posixSortTest.runTest(logger);



    logger.PrintTimeTable(csortInt::Name);

    return 0;
}


