//
// Created by conniezhong on 2019/5/3.
//

#ifndef HOTURL_SCHEDULER_H
#define HOTURL_SCHEDULER_H

#include <atomic>
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

    Scheduler();

public:
    int reportMapTaskFinishedOne() {
        _mapTaskFinishedNum++;
        return SUCCESS;
    }

    int reportMapTaskCreatedOne() {
        _mapTaskCreatedNum++;
        INFO << "report one map task, map task now:" << _mapTaskCreatedNum << endl;
        return SUCCESS;
    }

    int reportMapTaskCreatedOver() {
        _mapTaskCreateOver = true;
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
        INFO << "fetch task finished." << endl;
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
