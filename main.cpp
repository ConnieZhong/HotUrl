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
    if (argc != 10) {
        INFO << "input: input-path map-out-path "
             << "reduce-out-path fetch-out-path "
             << "worker-num buffer-per-line(show be 8 larger than url) "
             << "map-out-file-num k-op-top-k memory-byte " << endl;
        return PARM_ERROR;
    }

    auto start1 = chrono::steady_clock::now();



    int ret = Config::getInstance().init(
            argv[1],
            argv[2],
            argv[3],
            argv[4],
            atoi(argv[5]),
            atoi(argv[6]),
            atoi(argv[7]),
            atoi(argv[8]),
            atoi(argv[9])
    );

    /*"/home/conniezhong/data",
    "/home/conniezhong/map_out/",
    "/home/conniezhong/reduce_out/reduce_out_",
    "/home/conniezhong/fetch_out/fetch_out",
    2,
    1024,
    10,
    100,
    10 * 1024 * 1024*/
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

    while (!Scheduler::getInstance().fetchTaskFinished()) {
        sleep(1);
    }
    auto end1 = chrono::steady_clock::now();
    auto diff1 = end1 - start1;
    INFO << "time used:" << chrono::duration <double, milli>(diff1).count() << " ms" << endl;


}