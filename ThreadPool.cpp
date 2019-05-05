//
// Created by conniezhong on 2019/5/2.
//
#include "ThreadPool.h"
#include "Comm.h"

// the constructor just launches some amount of workers

int ThreadPool::init(size_t threads) {
    stop = false;
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
                [this] {
                    for (;;) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->_queueMutex);
                            this->_condition.wait(lock,
                                                 [this] { return this->stop || !this->_tasks.empty(); });
                            if (this->stop && this->_tasks.empty())
                                return SUCCESS;
                            task = std::move(this->_tasks.front());
                            this->_tasks.pop();
                        }

                        task();
                    }
                }
        );
    return SUCCESS;
};


// add new work item to the pool


// the destructor joins all threads
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        stop = true;
    }
    _condition.notify_all();
    for (std::thread &_worker: workers)
        _worker.join();
}