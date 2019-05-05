//
// Created by conniezhong on 2019/5/2.
//
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "MapTask.h"
#include "Config.h"
#include "ErrorCode.h"
#include "Scheduler.h"
#include "Comm.h"
#include "File.h"
#include "BufferManager.h"


using namespace std;

void MapTask::run() {
    //读取数据，进行hash到不同的文件中
    //DEBUG <<  "one map begin..., need buffer num:" << Config::getInstance().mapFileNum() << endl;
    unsigned int hashCode = 0;
    //创建map文件
    for (int i = 0; i < Config::getInstance().mapFileNum(); ++i) {
        shared_ptr<FileBuffer> fileBuffer;
        BufferManager::getInstance().getBuffer(fileBuffer);
        int status = fileBuffer->initFile(Config::getInstance().mapOutFilePrefix()
                                          + intToString(i).c_str(),
                                          O_WRONLY | O_APPEND | O_CREAT, MODE);
        if (status != SUCCESS) {
            ERROR << "init buffer for map err: file name:" << Config::getInstance().mapOutFilePrefix() + intToString(i)
                  << " status:" << status << endl;
            return;
        }
        _fdVec.push_back(fileBuffer);
    }
    Scheduler::getInstance().reportMapTaskReadyOne();
    //DEBUG <<  "map get buffer success" << endl;


    //TODO 文件还是太大了还需要进行拆解
    for (int i = 0; i < _bufferPtr->lineNow(); ++i) {
        char *data;
        int ret = _bufferPtr->getLine(i, &data);
        if (ret != SUCCESS) {
            ERROR << "get line err. ret:" << ret << endl;
            return;
        }
        hashCode = hotUrlHash(data) % Config::getInstance().mapFileNum();
        ret = _fdVec[hashCode]->addLine(data);
        if (ret != SUCCESS) {
            ERROR << "add line err, ret:" << ret << endl;
            return;
        }

        if (_fdVec[hashCode]->isFull()) {
            ret = _fdVec[hashCode]->writeToFile();
            if (ret != SUCCESS) {
                ERROR << "write file err. ret:" << ret << endl;
                return;
            }
            //DEBUG <<  "file name:" << _fdVec[hashCode]->getFileName() << " write one time" << endl;
            ret = _fdVec[hashCode]->clear();
            if (ret != SUCCESS) {
                ERROR << "clear buffer err. ret:" << ret << endl;
                return;
            }
        }

        ////DEBUG <<  "data :" << data << " will write to:" << hashCode << endl;
    }
    for (auto it:_fdVec) {
        if (!it->isEmpty()) {
            int ret = it->writeToFile();
            if (ret != SUCCESS) {
                ERROR << "write file err. ret:" << ret << endl;
                return;
            }
            //DEBUG <<  "file name:" << it->getFileName() << " write one time" << endl;
        }

    }

    Scheduler::getInstance().reportMapTaskFinishedOne();
}

MapTask::~MapTask() {
    BufferManager::getInstance().releaseBuffer(_bufferPtr);
    for (auto it:_fdVec) {
        BufferManager::getInstance().releaseBuffer(it);
    }
}

void ReadFileTask::run() {
    //打开文件
    DIR *dir;
    struct dirent *ptr;

    string filePath = Config::getInstance().inputFilePath();
    if ((dir = opendir(filePath.c_str())) == NULL) {
        ERROR << "open dir:" << filePath << " err" << endl;
        return;
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
            continue;
        else if (/*ptr->d_type == 8*/ 1) {
            INFO << "read task begin process file" << ptr->d_name << endl;
            int ret = readFileToTask(Config::getInstance().inputFilePath() + "/" + ptr->d_name);
            if (ret != SUCCESS) {
                INFO << "read file to task err. file:" << ptr->d_name << " ret: " << ret << endl;
                closedir(dir);
                return;
            }
            INFO << "file:" << Config::getInstance().inputFilePath() << "/" << ptr->d_name << " read over" << endl;
        } else {
            INFO << "file type:" << ptr->d_type << " file name:" << ptr->d_name
                 << " will not be processed." << endl;
        }

    }

    //向scheduler报告
    Scheduler::getInstance().reportMapTaskCreatedOver();
    closedir(dir);
}

int ReadFileTask::readFileToTask(string fileName) {
    ifstream in(fileName);
    if (!in.is_open()) {
        ERROR << "open file:" << fileName << " err" << endl;
        return READ_FILE_ERR;
    }

    shared_ptr<BaseBuffer> baseBuffer;
    BufferManager::getInstance().getBuffer(baseBuffer);
    shared_ptr<char> data = make_shared_array<char>(Config::getInstance().bufferSizePerline());
    while (!in.eof()) {
        //TODO 比较不同的接口的效率
        void *tmp = memset(data.get(), Config::getInstance().bufferSizePerline(), 0);
        if (tmp == NULL) {
            ERROR << "reset buff err" << endl;
            return MEMORY_ERROR;
        }
        in.getline(data.get(), Config::getInstance().bufferSizePerline() - 8);
        if (in.eof()) {
            INFO << "file get over. file name: " << fileName << endl;
            break;
        }
        ////DEBUG <<  "get line is:" << data.get() << ":" << Config::getInstance().bufferSizePerline() << endl;
        int ret = baseBuffer->addLine(data.get());
        if (ret != 0) {
            ERROR << "add line error" << endl;
            return ret;
        }

        if (baseBuffer->isFull()) {
            shared_ptr<MapTask> mapTask = make_shared<MapTask>();
            mapTask->setBufferPtr(baseBuffer);

            ThreadPool::getInstance().addTask(mapTask);

            Scheduler::getInstance().reportMapTaskCreatedOne();
            BufferManager::getInstance().getBuffer(baseBuffer);
        }
    }
    INFO << "file read over. file:" << fileName << endl;
    if (!baseBuffer->isEmpty()) {
        shared_ptr<MapTask> mapTask = make_shared<MapTask>();
        mapTask->setBufferPtr(baseBuffer);

        ThreadPool::getInstance().addTask(mapTask);
        Scheduler::getInstance().reportMapTaskCreatedOne();
    }
    in.close();
    return SUCCESS;
}
