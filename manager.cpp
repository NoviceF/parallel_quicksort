

#include "manager.h"
#include "sortimpl.h"

//pthread_mutex_t Manager::mutexQue_ = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t Manager::cond = PTHREAD_COND_INITIALIZER;

//Manager::Manager(int threadsTotal) : threadFree_(0), jobEnd_(0),
//threadsTotal_(threadsTotal) { }

//void Manager::addWork(int vecSize)
//{
//    int vecStart = 0;
//    pthread_mutex_lock(&mutexQue_);
//    jobEnd_ = false;
//    coordsDeq.clear();
//    coordsDeq.push_back(Coords{vecStart, vecSize - 1, 0});
//    pthread_mutex_unlock(&mutexQue_);
//}

void * f(void * arg)
{
    SortingTask *sortingTask = (static_cast<SortingTask*> (arg));
    Manager &mn = *sortingTask->mn;
    std::vector<int>& vec = mn.vecArr[sortingTask->id];
    std::deque<Coords> coordsDeq;
    coordsDeq.push_back(Coords{0, vec.size() - 1, 0});

    while (1)
    {
        Coords cd1;
        if (!coordsDeq.empty()) cd1 = coordsDeq.front();
        else pthread_exit(NULL);

        coordsDeq.pop_front();

        quicksort(vec, &cd1.start, &cd1.end, &cd1.i);

        //                cout << "i after qsort = " << cd1.i << endl;
        if (cd1.start < cd1.i - 1) coordsDeq.push_back(
                Coords{cd1.start, cd1.i - 1, cd1.i}
        );
        if (cd1.i + 1 < cd1.end) coordsDeq.push_back(
                Coords{cd1.i + 1, cd1.end, cd1.i}
        );
    }
    return NULL;
}

void * fmerge(void * arg)
{
    VecToMerge& vecs = *(static_cast<VecToMerge*> (arg));
//    std::vector<int> temp;
//    vecs.result.reserve(vecs.temp1->size()+vecs.temp2->size());
    
//    cout << "temp1.size in fmerge in thr #" << vecs.id << " = "
//            << vecs.temp1->size() << endl;
//    cout << "temp2.size in fmerge in thr #" << vecs.id << " = "
//            << vecs.temp2->size() << endl;
    std::merge(vecs.temp1->begin(), vecs.temp1->end(),
            vecs.temp2->begin(), vecs.temp2->end(), 
            std::back_inserter(vecs.result));
//    cout << "temp.size = " << temp.size() << endl;
//            std::back_inserter(vecs.result); vecs.result
    pthread_exit(NULL);
}

//void Manager::threadFree()
//{
//    ++threadFree_;
//}
//
//void Manager::threadBusy()
//{
//    --threadFree_;
//}
//
//bool Manager::isJobEnd()
//{
//    return jobEnd_;
//}
//
//void Manager::jobIsEnd()
//{
//    jobEnd_ = true;
//}
//
//bool Manager::isAllTreadsWait()
//{
//    //    pthread_mutex_lock(&mutexQue_);
//    //    int temp = threadFree_;
//    //    pthread_mutex_unlock(&mutexQue_);
//
//    return (threadsTotal_ == threadFree_);
//}
//
//void Manager::exitThread()
//{
//    pthread_cond_signal(&cond);
//    //    cout << "jobEnd" << endl;
//    jobIsEnd();
//    pthread_mutex_unlock(&mutexQue_);
//    pthread_exit(NULL);
//}
//
//void Manager::reSet(int threadCount)
//{
//    coordsDeq.clear();
//    vec.clear();
//    threadFree_ = 0;
//    jobEnd_ = false;
//    threadsTotal_ = threadCount;
//}

void Manager::initVec(int vecSize)
{
    srand(time(NULL));
    for (int i = 0; i < vecSize; ++i)
    {
        vec.push_back(rand() % 100000);
    }
}

void Manager::initVecHalf(int vecSize)
{
    for (int i = 0; i < vecSize / 2; ++i)
    {
        vec.push_back(i);
    }
    srand(time(NULL));
    for (int i = vecSize / 2 + 1; i < vecSize; ++i)
    {
        vec.push_back(rand() % 100);
    }

}

void Manager::printVec(int vecSize)
{
    for (int i = 0; i < vecSize; ++i)
    {
        std::cout << vec[i] << " ";
    }
    cout << endl;
}

