//
// Created by conniezhong on 2019/5/3.
//

#ifndef HOTURL_REDUCETASK_H
#define HOTURL_REDUCETASK_H

#include "ThreadPool.h"
#include "BufferManager.h"


using namespace std;

class ReduceTask : public BaseTask {
private:
    string _mapFileName;
    shared_ptr<FileBuffer> _fileBuffer;
public:
    void run();
    void setMapFileName(string name){
        _mapFileName = name;
    }
    ~ReduceTask();

};

#endif //HOTURL_REDUCETASK_H
