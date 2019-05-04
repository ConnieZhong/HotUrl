//
// Created by conniezhong on 2019/5/2.
//

#ifndef HOTURL_THREADPOOL_H
#define HOTURL_THREADPOOL_H

#include <queue>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>
#include <atomic>
#include <pthread.h>

using namespace std;

class BaseTask {
public:
    //可以继承这个类重写该方法执行任务
    //TODO 添加信号量处理方式
    virtual void run()=0;
};

class Thread {
private:
    thread _thread;
    atomic_bool _isFree;
    shared_ptr<BaseTask> _task;
    mutex _mutex;
public:
    //构造
    Thread() : _isFree(true), _task(nullptr) {
        _thread = thread(&Thread::run, this);
        _thread.detach();
    }

    //是否空闲
    bool isFree() {
        return _isFree;
    }

    //添加任务
    void addTask(shared_ptr<BaseTask> task);

    //如果有任务则执行任务，否则自旋
    void run();

};

class ThreadPool {
private:
    queue<shared_ptr<BaseTask>> task_queue;
    vector<shared_ptr<Thread> > _pool;
    mutex _mutex;


public:
    int init(int threadNum);
    static ThreadPool &getInstance() {
        static ThreadPool instance;
        return instance;
    }

    //释放线程池
    virtual  ~ThreadPool();

    //添加任务
    void addTask(shared_ptr<BaseTask> task);

    //轮询
    void run();

};

#endif //HOTURL_THREADPOOL_H
