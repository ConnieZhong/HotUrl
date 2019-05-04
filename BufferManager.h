//
// Created by conniezhong on 2019/5/3.
//

#ifndef HOTURL_BUFFERMANAGER_H
#define HOTURL_BUFFERMANAGER_H

#include <memory>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "File.h"
#include "Comm.h"

using namespace std;

class BaseBuffer {
protected:
    shared_ptr<char> _data;
    int _sizeCapacityPerLine;
    int _lineNumCapacity;
    int _lineNow;
    int _status;

public:
    int init(int spl, int ln);

    int addLine(char *);

    int getLine(int, char **);

    int clear();

    bool isFull();

    bool isEmpty();

    int status() {
        return _status;
    }

    int lineNow() {
        return _lineNow;
    }

    int addLine(const char *, int);

    int getLine(int, char **, int &);

    void printInfo() {
        INFO << "addr:" << (long) (_data.get())
             <<" line now:" << _lineNow << endl;
    }

};

class FileBuffer : public BaseBuffer {
private:
    File _file;

public:
    //FileBuffer(string fileName, int mod, int spl, int ln);

    int initFile(string fileName, int mod);

    int writeToFile();

    int readFromFile();
};

//TODO .h .cpp分开
class BufferManager {
private:
    BufferManager() {
    }

    condition_variable _cond;
    queue<shared_ptr<FileBuffer>> _que;
    atomic_int _totalBufferNum;
    mutex _mutex;
public:
    static BufferManager &getInstance() {
        static BufferManager instance;
        return instance;
    }

    int init(int totalBuffer);

    template<typename T>
    void getBuffer(shared_ptr<T> &ptr) {
        std::unique_lock<mutex> ulk(_mutex);                    // 3.全局加锁
        _cond.wait(ulk, [this]() {
                       return !_que.empty();
                   }
        );
        ptr = static_pointer_cast<T>(_que.front());
        _que.pop();
    }

    //TODO lock 原理
    void releaseBuffer(shared_ptr<BaseBuffer> ptr) {
        std::lock_guard<mutex> lk(_mutex);            // 1.全局加锁
        _que.push(static_pointer_cast<FileBuffer>(ptr));           // 2.push时独占锁
        _cond.notify_one();
    }
};


#endif //HOTURL_BUFFERMANAGER_H
