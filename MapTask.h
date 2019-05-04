//
// Created by conniezhong on 2019/5/2.
//

#ifndef HOTURL_GETFILETASK_H
#define HOTURL_GETFILETASK_H

#include "ThreadPool.h"
#include "BufferManager.h"

class MapTask : public BaseTask {
public:
    ~MapTask();
    void setBufferPtr (shared_ptr<BaseBuffer> ptr){
        _bufferPtr = ptr;
    }
    void run();
private:
    shared_ptr<BaseBuffer> _bufferPtr;
    vector<shared_ptr<FileBuffer>> _fdVec;

};

class GetFileTask : public BaseTask {
public:
    void run();
private:
    int readFileToTask(string fileName);


};

#endif //HOTURL_GETFILETASK_H
