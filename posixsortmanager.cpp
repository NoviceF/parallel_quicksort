#include "posixsortmanager.h"
#include "quicksortimpl.h"
#include "profiler.h"

pthread_mutex_t Manager::mutexQue = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Manager::cond = PTHREAD_COND_INITIALIZER;

Manager::Manager(std::vector<int>& sourceVec, int threadsTotal)
    : vec_(sourceVec)
    , waitOnCond_(0)
    , jobEnd_(0)
    , threadsTotal_(threadsTotal) { }

void Manager::addWork(size_t vecSize)
{
    size_t vecStart = 0;
    pthread_mutex_lock(&mutexQue);
    jobEnd_ = false;

    globalDeq_.push(Coords{vecStart, vecSize - 1, 0});

    pthread_mutex_unlock(&mutexQue);
}

void * f(void * arg)
{
    SortingTask *sortingTask = (static_cast<SortingTask*> (arg));
    Manager &mn = *sortingTask->mn;
    std::stack<Coords> localDeq;

    while (true)
    {
        if (mn.globalDeq().empty() && localDeq.empty()) ///////// waiting cond
        {
            Lock mLock(mn.mutexQue);

            while (mn.globalDeq().empty() && !mn.isOtherTreadsWait())
            {
                ++mn.waitOnCond();
                pthread_cond_wait(&mn.cond, &mn.mutexQue);

                if (mn.jobEnd())
                {
                    mn.exitThread();
                }

                --mn.waitOnCond();
            }

            //////// exit cond
            if (mn.globalDeq().empty() && localDeq.empty() && mn.isOtherTreadsWait())
                mn.exitThread();
        }

        ////sorting from localdeq
        if (localDeq.empty() && !mn.globalDeq().empty()) ////// localdeq is empty
        {
            Lock mLock(mn.mutexQue);

            if (!mn.globalDeq().empty())
            {
                localDeq.push(mn.globalDeq().top());
                mn.globalDeq().pop();
            }
        }

        while (!localDeq.empty())
        {
            Coords cd1 = localDeq.top();
            localDeq.pop();

            quicksort(mn.vec(), cd1.start, cd1.end, &cd1.pivot);

            if (cd1.pivot && cd1.start < cd1.pivot - 1) localDeq.push(
                    Coords{cd1.start, cd1.pivot - 1, cd1.pivot}
            );

            if (cd1.pivot + 1 < cd1.end) localDeq.push(
                    Coords{cd1.pivot + 1, cd1.end, cd1.pivot}
            );

            if (mn.globalDeq().empty() && localDeq.size() > 1)//!!!!!!!!!!!
            {
                break;
            }
        }

        //////////////// globalDeq is empty
        if (mn.globalDeq().empty() && localDeq.size() > 1)//!!!!!!!!!!!!
        {
            Lock mLock(mn.mutexQue);

            if (mn.globalDeq().empty())
            {
                mn.globalDeq().push(localDeq.top());
                localDeq.pop();
                if (mn.waitOnCond())
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
    pthread_mutex_unlock(&mutexQue);
    pthread_exit(NULL);
}

void Manager::reset(int threadCount)
{
    vec_.clear();

    jobEnd_ = false;
    threadsTotal_ = threadCount;
}
std::stack<Coords>& Manager::globalDeq()
{
    return globalDeq_;
}
volatile int& Manager::waitOnCond()
{
    return waitOnCond_;
}
volatile bool& Manager::jobEnd()
{
    return jobEnd_;
}
std::vector<int>& Manager::vec()
{
    return vec_;
}
