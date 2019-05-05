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
             << " line now:" << _lineNow << endl;
    }


};

class FileBuffer : public BaseBuffer {
private:
    File _file;
    int _outerFd;

public:
    //FileBuffer(string fileName, int mod, int spl, int ln);

    int initFile(string fileName, int mod, int p = MODE);

    int writeToFile();

    int writeToOuterFd();

    int readFromFile();

    string getFileName() {
        return _file.fileName();
    }

    void fileClose() {
        _file.fileClose();
    }
    void setOuterFd(int fd){
        _outerFd = fd;
    }

};

class BufferManager {
private:
    BufferManager() {
    }

    ~BufferManager(){
        for(auto it:_mapOutFile){
            close(it);
        }
    }

    condition_variable _cond;
    mutex _mutex;

    queue<shared_ptr<FileBuffer>> _que;
    atomic_int _totalBufferNum;
    vector<int> _mapOutFile;
public:
    static BufferManager &getInstance() {
        static BufferManager instance;
        return instance;
    }

    int init(int totalBuffer);

    template<typename T>
    void getBuffer(shared_ptr<T> &ptr) {
        std::unique_lock<mutex> ulk(_mutex);
        _cond.wait(ulk, [this]() {
                       return !_que.empty();
                   }
        );
        ptr = static_pointer_cast<T>(_que.front());
        static_pointer_cast<BaseBuffer>(ptr)->clear();
        _que.pop();
        if (_que.size() == 0) {
            DEBUG << "...left buffer num:" << _que.size() << endl;
        }
    }

    void releaseBuffer(shared_ptr<BaseBuffer> ptr) {
        {
            std::lock_guard<mutex> lk(_mutex);
            ptr->clear();
            static_pointer_cast<FileBuffer>(ptr)->fileClose();
            _que.push(static_pointer_cast<FileBuffer>(ptr));

        }
        _cond.notify_one();
    }

    int getOuterFd(size_t i){
        return _mapOutFile[i];
    }

};


#endif //HOTURL_BUFFERMANAGER_H
