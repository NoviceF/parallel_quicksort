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
    //    globalDeq.clear();
    globalDeq.push(Coords{vecStart, vecSize - 1, 0});
    //    cout << "after adding data globalDeqsize = " << globalDeq.size() << endl;
    pthread_mutex_unlock(&mutexQue_);
}

void * f(void * arg)
{
    SortingTask *sortingTask = (static_cast<SortingTask*> (arg));
    Manager &mn = *sortingTask->mn;
    std::stack<Coords> localDeq;

    while (1)
    {
        //        if (sortingTask->id == 1) cout << "globalDeqsize in thread " <<
        //                sortingTask->id << " = " << mn.globalDeq.size() << endl;
        //        if (sortingTask->id == 1) cout << "localDeqsize in thread " <<
        //                sortingTask->id << " = " << localDeq.size() << endl;

        //        ptime t1;
        //        t1 = microsec_clock::local_time();
        //        cout << "time in thread "
        //                << sortingTask->id << " = " << t1.time_of_day() << endl;
        //        if (sortingTask->id == 1 && !mn.jobEnd_)
        //        {
        //            while (mn.globalDeq.empty())
        //            {
        //                cout << "test in thr 1" << endl;
        //                sleep(2);
        //            }
        //        }

        if (mn.globalDeq.empty() && localDeq.empty()) ///////// waiting cond
        {
            Lock mLock(mn.mutexQue_);
            //            pthread_mutex_lock(&mn.mutexQue_);
            while (mn.globalDeq.empty() && !mn.isOtherTreadsWait())
            {
                ++mn.waitOnCond_;
                //                if (sortingTask->id == 1)
                //                {
                //                cout << "waiting on cond in thread "
                //                        << sortingTask->id << endl;
                //                    cout << "waitOnCond in thr 1 = " << mn.waitOnCond_ << endl;
                //                }
                pthread_cond_wait(&mn.cond, &mn.mutexQue_);
                //                cout << "awake from waiting on cond in thread " <<
                //                        sortingTask->id << endl;
                if (mn.jobEnd_)
                {
                    mn.exitThread();
                }
                //                    cout << "localDeqsize = " << localDeq.size() << endl;
                //                    cout << "globalDeqsize = " << mn.globalDeq.size() << endl;
                --mn.waitOnCond_;
            }
            if (mn.globalDeq.empty() && localDeq.empty() //////// exit cond
                    && mn.isOtherTreadsWait()) mn.exitThread();
            //            pthread_mutex_unlock(&mn.mutexQue_);
        }

        //        sleep(1);


        ////sorting from localdeq
        if (localDeq.empty() && !mn.globalDeq.empty()) ////// localdeq is empty
        {
            Lock mLock(mn.mutexQue_);
            //            pthread_mutex_lock(&mn.mutexQue_);
            if (!mn.globalDeq.empty())
            {
                //                if (mn.globalDeq.size() >= 2)
                //                {
                //                    cout << "work2!" << endl;
                //                    for (int i = 0; i < 2; ++i)
                //                    {
                //                        localDeq.push(mn.globalDeq.top());
                //                        mn.globalDeq.pop();
                //                    }
                //                }
                localDeq.push(mn.globalDeq.top());
                mn.globalDeq.pop();
            }
            //            pthread_mutex_unlock(&mn.mutexQue_);
            //            cout << "get data from globalDeq in thread " <<
            //                    sortingTask->id << endl;
            //            sleep(1);
            //
            //
            //            cout << "globalDeqsize in thread " <<
            //                    sortingTask->id << " = " << mn.globalDeq.size() << endl;
            //            cout << "localDeqsize in thread " <<
            //                    sortingTask->id << " = " << localDeq.size() << endl;
        }

        while (!localDeq.empty())
        {
            //            cout << "sorting in thread " <<
            //                    sortingTask->id << endl;
            Coords cd1 = localDeq.top();
            localDeq.pop();

            //            if (cd1.end - 1 <= M)
            //            {
            //                //                cout << "insertation!" << endl;
            //                insertion(mn.vec, &cd1.start, &cd1.end);
            //            } else
            //            {
            quicksort(mn.vec, &cd1.start, &cd1.end, &cd1.i);


            if (cd1.start < cd1.i - 1) localDeq.push(
                    Coords{cd1.start, cd1.i - 1, cd1.i}
            );
            if (cd1.i + 1 < cd1.end) localDeq.push(
                    Coords{cd1.i + 1, cd1.end, cd1.i}
            );
            //            }
            //            cout << "localDeqsize in thread " <<
            //                    sortingTask->id << " = " << localDeq.size() << endl;
            //            cout << "globalDeqsize in thread " <<
            //                    sortingTask->id << " = " << mn.globalDeq.size() << endl;

            if (mn.globalDeq.empty() && localDeq.size() > 1)//!!!!!!!!!!!
            {
                //                cout << "globalDeq is empty in thread " <<
                //                        sortingTask->id << " break" << endl;
                break;
            }
        }
        //        sleep(1);

        //                        if (sortingTask->id == 0) sleep(1);

        //////////////// globalDeq is empty
        if (mn.globalDeq.empty() && localDeq.size() > 1)//!!!!!!!!!!!!
        {
            Lock mLock(mn.mutexQue_);
            //            pthread_mutex_lock(&mn.mutexQue_);
            if (mn.globalDeq.empty())
            {
                //                cout << "pull data from local to global in thread " <<
                //                        sortingTask->id << " wait on cond = " << mn.waitOnCond_
                //                        << endl;
                //                if (localDeq.size() > 2)
                //                {
                //                    cout << "work1 !" << endl;
                //                    for (int i = 0; i < 2; ++i)
                //                    {
                //                        mn.globalDeq.push(localDeq.top());
                //                        localDeq.pop();
                //                    }
                //                    }
                mn.globalDeq.push(localDeq.top());
                localDeq.pop();
                if (mn.waitOnCond_)
                {
                    pthread_cond_signal(&mn.cond);
                    //                    cout << "transmit signal in thread " <<
                    //                            sortingTask->id << endl;
                }
            }
            //            pthread_mutex_unlock(&mn.mutexQue_);
            //                                continue; ///!!!!!!!!
            //            break;
        }
        //        cout << "test in thread " <<
        //                sortingTask->id << endl;

    }
    return NULL;
}

//void Manager::threadFree()
//{
//
//    ++threadFree_;
//}

//void Manager::threadBusy()
//{
//
//    --threadFree_;
//}

bool Manager::isJobEnd()
{

    return jobEnd_;
}

void Manager::jobIsEnd()
{

    jobEnd_ = true;
}

bool Manager::isOtherTreadsWait()
{
    //    pthread_mutex_lock(&mutexQue_);
    //    int temp = threadFree_;
    //    pthread_mutex_unlock(&mutexQue_);

    return waitOnCond_ == (threadsTotal_ - 1);
}

void Manager::exitThread()
{
    jobIsEnd();
    //    cout << "jobEnd" << endl;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutexQue_);
    pthread_exit(NULL);
}

void Manager::reSet(int threadCount)
{

    //    globalDeq.clear();
    vec.clear();
    //    threadFree_ = 0;
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
    cout << endl;
}
