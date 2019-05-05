//
// Created by conniezhong on 2019/5/3.
//

#ifndef HOTURL_SCHEDULER_H
#define HOTURL_SCHEDULER_H

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "ErrorCode.h"
#include "ThreadPool.h"
#include "FetchTask.h"
#include "Comm.h"

class Scheduler {
private:
    atomic_int _mapTaskCreatedNum;
    atomic_int _mapTaskFinishedNum;
    atomic_bool _mapTaskCreateOver;

    atomic_int _reduceTaskCreatedNum;
    atomic_int _reduceTaskFinishedNum;
    atomic_bool _reduceTaskCreateOver;

    atomic_bool _fetchTaskFinied;
    bool _canReadNext;
    condition_variable _cond;
    mutex _mutex;

    Scheduler();

public:

    int reportMapTaskFinishedOne() {
        _mapTaskFinishedNum++;
        //INFO << "+++++++ map finish one, finished num:" << _mapTaskFinishedNum
        //     << " +++++++"<< endl;

        if (_mapTaskCreateOver) {
            INFO << "+++++++ map task left:"
                 << _mapTaskCreatedNum - _mapTaskFinishedNum << " +++++++"
                 << endl;
        }

        if ((_mapTaskCreatedNum == _mapTaskFinishedNum) && _mapTaskCreateOver) {
            int ret = beginReduce();
            if (ret != SUCCESS) {
                ERROR << "begin reduce err. ret:" << ret << endl;
                return ret;
            }
        }

        return SUCCESS;
    }

    int reportMapTaskCreatedOne() {
        _mapTaskCreatedNum++;
        //INFO << "+++++++ map task create one, map task now:" << _mapTaskCreatedNum << " +++++++"<<endl;
        //等待条件变量，不然会导致buffer耗尽
        std::unique_lock<mutex> ulk(_mutex);                    // 3.全局加锁
        _canReadNext = false;
        _cond.wait(ulk, [this]() {
                       return _canReadNext;
                   }
        );
        ulk.unlock();
        return SUCCESS;
    }

    int reportMapTaskReadyOne() {
        std::lock_guard<mutex> lk(_mutex);
        _canReadNext = true;
        _cond.notify_one();
    }

    int reportMapTaskCreatedOver() {
        _mapTaskCreateOver = true;
        INFO << "+++++++ map task create over,total num:" << _mapTaskCreatedNum << " +++++++" << endl;

        if ((_mapTaskCreatedNum == _mapTaskFinishedNum) && _mapTaskCreateOver) {
            int ret = beginReduce();
            if (ret != SUCCESS) {
                ERROR << "begin reduce err. ret:" << ret << endl;
                return ret;
            }
        }
        return SUCCESS;
    }

    int reportReduceTaskFinishedOne() {
        _reduceTaskFinishedNum++;
        if ((_reduceTaskFinishedNum == _reduceTaskCreatedNum) && _reduceTaskCreateOver) {
            shared_ptr<FetchTask> fetchTask = make_shared<FetchTask>();
            ThreadPool::getInstance().addTask(fetchTask);
        }
        return SUCCESS;
    }

    int reportReduceTaskCreatedOne() {
        _reduceTaskCreatedNum++;

        return SUCCESS;
    }

    int reportReduceTaskCreatedOver() {
        _reduceTaskCreateOver = true;
        if (_reduceTaskFinishedNum == _reduceTaskCreatedNum && _reduceTaskCreateOver) {
            shared_ptr<FetchTask> fetchTask = make_shared<FetchTask>();
            ThreadPool::getInstance().addTask(fetchTask);
        }
        return SUCCESS;
    }

    int reportFetchTaskFinished() {
        _fetchTaskFinied = true;
        INFO << "+++++++ fetch task finished +++++++" << endl;
        return SUCCESS;
    }

    bool fetchTaskFinished() {
        return _fetchTaskFinied;
    }

    int beginReduce();

    static Scheduler &getInstance() {
        static Scheduler instance;
        return instance;
    }

    int begin();

};

#endif //HOTURL_SCHEDULER_H
