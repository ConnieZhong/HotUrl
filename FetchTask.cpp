//
// Created by conniezhong on 2019/5/3.
//
#include <fstream>
#include "FetchTask.h"
#include "Config.h"
#include "File.h"
#include "Comm.h"
#include "ErrorCode.h"
#include "Scheduler.h"
#include "BufferManager.h"

void FetchTask::run() {
    //批量从文件中读取数据
    //批量读取
    int ret = 1;
    shared_ptr<FileBuffer> fileBuffer;
    BufferManager::getInstance().getBuffer(fileBuffer);
    ret = fileBuffer->initFile(Config::getInstance().reduceOutFileName(), O_RDONLY);
    if (ret != 0) {
        ERROR << "fetch err. ret:" << endl;
        return;
    }

    vector<pair<int, string>> vec(100, make_pair(0, ""));
    while (true) {
        ret = fileBuffer->readFromFile();
        if (ret == 0) {
            INFO << "file:" << Config::getInstance().reduceOutFileName() << " read over" << endl;
            break;
        }

        if (ret <= 0) {
            ERROR << "file:" << Config::getInstance().reduceOutFileName() << " read err. ret:" << ret << endl;
            return;
        }

        int lineNow = fileBuffer->lineNow();

        for (int i = 0; i < lineNow; ++i) {
            char *data;
            int num;
            ret = fileBuffer->getLine(i, &data, num);
            if (ret != SUCCESS) {
                ERROR << "read buffer err. ret:" << ret << " file name:" << Config::getInstance().reduceOutFileName() << endl;
                return;
            }
            ////DEBUG <<  "fetch data: url:" << data << " num:" << num << endl;
            if (num > vec[0].first) {
                vec[0].first = num;
                vec[0].second = data;
                sort_heap(vec.begin(), vec.end());
            }
        }
    }

    fstream f(Config::getInstance().fetchOutFileName(),ios::out);
    for (auto it: vec) {
        //INFO << "result is:" << it.first << ": " << it.second << endl;
        f<< it.first << "|" << it.second <<endl;
    }
    f.close();
    Scheduler::getInstance().reportFetchTaskFinished();

}

