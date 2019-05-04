#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "Scheduler.h"
#include "ThreadPool.h"
#include "MapTask.h"
#include "Comm.h"
#include "Config.h"

using namespace std;


int main(int argc, char *argv[]) {
    int ret = Config::getInstance().init(
            "/home/conniezhong/data",
            "/home/conniezhong/map_out/",
            "/home/conniezhong/reduce_out/reduce_out_",
            2,
            1024,
            10,
            100,
            10*1024*1024
    );

    if (ret != SUCCESS) {
        ERROR << "init err. ret:" << ret << endl;
        return ret;
    }

    ret = ThreadPool::getInstance().init(Config::getInstance().workerThreadNum());
    if (ret != SUCCESS) {
        ERROR << "thread pool init err" << endl;
        return INNER_ERROR;
    }

    ret = BufferManager::getInstance().init(Config::getInstance().bufferNum());
    if (ret != SUCCESS) {
        ERROR << "buffer manager init err" << endl;
        return INNER_ERROR;
    }

    ret = Scheduler::getInstance().begin();
    if (ret != SUCCESS) {
        ERROR << "scheduler begin err" << endl;
        return INNER_ERROR;
    }

    while (!Scheduler::getInstance().fetchTaskFinished()){
        sleep(4);
    }
}