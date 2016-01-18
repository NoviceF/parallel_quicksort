#include "manager.h"
#include "sortimpl.h"
#include "profiler.h"

pthread_mutex_t Manager::mutexQue_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Manager::cond = PTHREAD_COND_INITIALIZER;

Manager::Manager(int threadsTotal) : waitOnCond_(0), jobEnd_(0),
threadsTotal_(threadsTotal) { }

void Manager::addWork(int vecSize)
{
    int vecStart = 0;
    pthread_mutex_lock(&mutexQue_);
    jobEnd_ = false;

    globalDeq.push(Coords{vecStart, vecSize - 1, 0});

    pthread_mutex_unlock(&mutexQue_);
}

void * f(void * arg)
{
    SortingTask *sortingTask = (static_cast<SortingTask*> (arg));
    Manager &mn = *sortingTask->mn;
    std::stack<Coords> localDeq;

    while (true)
    {
        if (mn.globalDeq.empty() && localDeq.empty()) ///////// waiting cond
        {
            Lock mLock(mn.mutexQue_);

            while (mn.globalDeq.empty() && !mn.isOtherTreadsWait())
            {
                ++mn.waitOnCond_;
                pthread_cond_wait(&mn.cond, &mn.mutexQue_);

                if (mn.jobEnd_)
                {
                    mn.exitThread();
                }

                --mn.waitOnCond_;
            }

            //////// exit cond
            if (mn.globalDeq.empty() && localDeq.empty() && mn.isOtherTreadsWait())
                mn.exitThread();
        }

        ////sorting from localdeq
        if (localDeq.empty() && !mn.globalDeq.empty()) ////// localdeq is empty
        {
            Lock mLock(mn.mutexQue_);

            if (!mn.globalDeq.empty())
            {
                localDeq.push(mn.globalDeq.top());
                mn.globalDeq.pop();
            }
        }

        while (!localDeq.empty())
        {
            Coords cd1 = localDeq.top();
            localDeq.pop();

            quicksort(mn.vec, &cd1.start, &cd1.end, &cd1.pivot);

            if (cd1.start < cd1.pivot - 1) localDeq.push(
                    Coords{cd1.start, cd1.pivot - 1, cd1.pivot}
            );

            if (cd1.pivot + 1 < cd1.end) localDeq.push(
                    Coords{cd1.pivot + 1, cd1.end, cd1.pivot}
            );

            if (mn.globalDeq.empty() && localDeq.size() > 1)//!!!!!!!!!!!
            {
                break;
            }
        }

        //////////////// globalDeq is empty
        if (mn.globalDeq.empty() && localDeq.size() > 1)//!!!!!!!!!!!!
        {
            Lock mLock(mn.mutexQue_);

            if (mn.globalDeq.empty())
            {
                mn.globalDeq.push(localDeq.top());
                localDeq.pop();
                if (mn.waitOnCond_)
                {
                    pthread_cond_signal(&mn.cond);
                }
            }
        }
    }

    return NULL;
}

bool Manager::isJobDone()
{

    return jobEnd_;
}

void Manager::jobIsDone()
{

    jobEnd_ = true;
}

bool Manager::isOtherTreadsWait()
{
    return waitOnCond_ == (threadsTotal_ - 1);
}

void Manager::exitThread()
{
    jobIsDone();

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutexQue_);
    pthread_exit(NULL);
}

void Manager::reset(int threadCount)
{
    vec.clear();

    jobEnd_ = false;
    threadsTotal_ = threadCount;
}

void Manager::initVec(int vecSize)
{
    srand(time(NULL));

    for (int i = 0; i < vecSize; ++i)
    {
        vec.push_back(rand() % vecSize);
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

    std::cout << std::endl;
}
