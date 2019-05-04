//
// Created by conniezhong on 2019/5/2.
//
#include "ThreadPool.h"
#include "Comm.h"

void Thread::addTask(shared_ptr<BaseTask> task) {
    if (_isFree) {
        _isFree = false;
        _mutex.lock();
        _task = task;
        _mutex.unlock();
    }
}

void Thread::run() {
    while (true) {
        if (_task) {
            _mutex.lock();
            _isFree = false;
            _task->run();
            _isFree = true;
            _task = nullptr;
            _mutex.unlock();
        }
    }
}

int ThreadPool::init(int threadNum) {
    while (threadNum--) {
        shared_ptr<Thread> t = make_shared<Thread>();
        _pool.push_back(t);
    }
    thread main_thread(&ThreadPool::run, this);
    main_thread.detach();
}

ThreadPool::~ThreadPool() {

}

//TODO 修改为信号量的方式
void ThreadPool::addTask(shared_ptr<BaseTask> task) {
    _mutex.lock();
    task_queue.push(task);
    _mutex.unlock();
}

void ThreadPool::run() {
    while (true) {
        _mutex.lock();
        if (task_queue.empty()) {
            _mutex.unlock();
            continue;
        }
        // 寻找空闲线程执行任务
        for (int i = 0; i < _pool.size(); ++i) {
            if (_pool[i]->isFree()) {
                _pool[i]->addTask(task_queue.front());
                DEBUG << "======thread pool exec one task begin=====" << endl;
                task_queue.pop();
                break;
            }
        }
        _mutex.unlock();
    }
}