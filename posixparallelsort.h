#ifndef POSIXPARALLELSORT_H
#define POSIXPARALLELSORT_H

#include <string>
#include <vector>

#include "posixsortmanager.h"
#include "sortbase.h"


template <typename T>
class PosixParallelSort : public MultiThreadSort<T>
{
public:
    static const std::string Name;

    PosixParallelSort(std::vector<T>& vecToSort, Logger& logger)
        : MultiThreadSort<T>(vecToSort, logger)
    {}

    void runTask() override
    {
        const size_t vecSize = SortBase<T>::m_vecToSort.size();

        if (vecSize < 2)
            return;

        const size_t threadsCount = MultiThreadSort<T>::threadsCount();
        Manager manager(SortBase<T>::m_vecToSort, threadsCount);
        manager.addWork(vecSize);

        std::vector<pthread_t> threads(threadsCount);
        std::vector<SortingTask> tasks(threadsCount);

        for (int i = 0; i < threads.size(); ++i)
        {
            tasks[i].id = i;
            tasks[i].mn = &manager;
//            tasks[i].dataType = type;

            int status = pthread_create(&threads[i], NULL, f, &tasks[i]);
            if (status != 0)
            {
                std::cout << "thread starting failed" << std::endl;
            }
        }

        for (int i = 0; i < threadsCount; ++i)
        {
            pthread_join(threads[i], NULL);
        }
    }

    std::string name() const override
    {
        return PosixParallelSort::Name;
    }

//    void doSortString(int threadsCount, Logger& logger, std::string path)
//    {
//        stringType stringImpl;

//        switch (threadsCount)
//        {
//            case 1: stringImpl = pOnethread;
//                break;
//            case 2: stringImpl = pTwothreads;
//                break;
//            case 4: stringImpl = pFourthreads;
//                break;
//            default:
//            {
//                cout << "Unknown count of threads!";
//                return;
//            }
//        }
//        //"COPYING"
//        std::ifstream inputFile(path);
//        std::string str;
//        Manager mn1(threadsCount);

//        while (inputFile >> str) mn1.vecStr.push_back(str);

//        mn1.addWork(mn1.vecStr.size());

//        //    cout << "vecsize = " << vecSize << endl;

//        pthread_t *threads = new pthread_t[threadsCount];

//        Prof prof;
//        startAll(threads, threadsCount, mn1, SortingTask::string);
//        logger.GetTime(stringImpl, prof.StopAndGet());

//        CheckSorted(mn1.vecStr, mn1.vecStr.size());
//        //    for (int i = 0; i < mn1.vecStr.size(); ++i)
//        //    {
//        //        cout << mn1.vecStr[i] << " ";
//        //    }

//        delete []threads;
//    }
};

template <typename T>
const std::string PosixParallelSort<T>::Name = "Posix";


#endif // POSIXPARALLELSORT_H
