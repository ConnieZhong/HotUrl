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
    //���Լ̳��������д�÷���ִ������
    //TODO ����ź�������ʽ
    virtual void run()=0;
};

class Thread {
private:
    thread _thread;
    atomic_bool _isFree;
    shared_ptr<BaseTask> _task;
    mutex _mutex;
public:
    //����
    Thread() : _isFree(true), _task(nullptr) {
        _thread = thread(&Thread::run, this);
        _thread.detach();
    }

    //�Ƿ����
    bool isFree() {
        return _isFree;
    }

    //�������
    void addTask(shared_ptr<BaseTask> task);

    //�����������ִ�����񣬷�������
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

    //�ͷ��̳߳�
    virtual  ~ThreadPool();

    //�������
    void addTask(shared_ptr<BaseTask> task);

    //��ѯ
    void run();

};

#endif //HOTURL_THREADPOOL_H
