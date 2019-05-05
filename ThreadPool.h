#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "Comm.h"
#include "ErrorCode.h"

using namespace std;

class ThreadPool {
private:
    ThreadPool(){}

public:
    static ThreadPool &getInstance() {
        static ThreadPool instance;
        return instance;
    }
    int init(size_t);
    template<class F, class... Args>
    auto enqueue(F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;

    ~ThreadPool();

    int addTask(shared_ptr<BaseTask> ptr) {
        enqueue(doSomeThing, ptr);
        return SUCCESS;
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> _worker;
    // the task queue
    std::queue<std::function<void()> > _tasks;

    // synchronization
    std::mutex _queueMutex;
    std::condition_variable _condition;
    bool stop;
};

template<class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&... args)
-> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_queueMutex);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        _tasks.emplace([task]() { (*task)(); });
    }
    _condition.notify_one();
    return res;
}

#endif
