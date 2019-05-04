//
// Created by conniezhong on 2019/5/3.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include "Scheduler.h"
#include "MapTask.h"
#include "Config.h"
#include "ReduceTask.h"

Scheduler::Scheduler() : _mapTaskCreatedNum(0), _mapTaskFinishedNum(0), _mapTaskCreateOver(false),
                         _reduceTaskCreatedNum(0), _reduceTaskFinishedNum(0), _reduceTaskCreateOver(0),
                         _fetchTaskFinied(false),_canReadNext(false) {
}

int Scheduler::begin() {
    shared_ptr<ReadFileTask> getFileTask = make_shared<ReadFileTask>();
    if(getFileTask.get() == NULL){
        return MEMORY_ERROR;
    }
    ThreadPool::getInstance().addTask(getFileTask);
    return SUCCESS;
}

int Scheduler::beginReduce() {
    //打开文件
    INFO <<"==========reduce begin===========" <<endl;
    DIR *dir;
    struct dirent *ptr;

    string filePath = Config::getInstance().mapOutFilePrefix();
    if ((dir = opendir(filePath.c_str())) == NULL) {
        ERROR << "open dir:" << filePath << " err" << endl;
        return INNER_ERROR;
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 8) {
            INFO << "reduce begin process " << ptr->d_name << endl;
            shared_ptr<ReduceTask> reduceTask = make_shared<ReduceTask>();

            reduceTask->setMapFileName(filePath + "/" + ptr->d_name);
            ThreadPool::getInstance().addTask(reduceTask);
            Scheduler::getInstance().reportReduceTaskCreatedOne();
        } else {
            INFO << "file type:" << ptr->d_type << " file name:" << ptr->d_name
                 << " will not be processed." << endl;
        }
    }

    //向scheduler报告
    Scheduler::getInstance().reportReduceTaskCreatedOver();
    closedir(dir);
}