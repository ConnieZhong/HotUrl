//
// Created by conniezhong on 2019/5/2.
//

#ifndef HOTURL_CONFIG_H
#define HOTURL_CONFIG_H

#include <string>
#include "ErrorCode.h"
#include "Comm.h"

using namespace std;

class Config {
private:
    Config() {

    }

    string _inputFilePath;
    string _mapOutFilePrefix;
    string _reduceOutFileName;

    int _workerThreadNum;
    int _bufferSizePerline;
    int _bufferMaxLine;
    int _mapFileNum;
    int _kOfTopK;

public:

    int init(string i, string m, string r, int w, int ma,
             int b, int mf, int k) {
        _inputFilePath = i;
        _mapOutFilePrefix = m ;
        _reduceOutFileName = r;

        _workerThreadNum = w;
        _bufferSizePerline = ma;
        _bufferMaxLine = b;
        _mapFileNum = mf;
        _kOfTopK = k;

        if (_bufferMaxLine < _kOfTopK) {
            ERROR << "parm err." << endl;
            return PARM_ERROR;
        }
        return SUCCESS;
    }

    static Config &getInstance() {
        static Config instance;
        return instance;
    }

    string inputFilePath() {
        return _inputFilePath;
    }


    string mapOutFilePrefix() {
        return _mapOutFilePrefix;
    }

    string reduceOutFileName() {
        return _reduceOutFileName;
    }

    int workerThreadNum() {
        return _workerThreadNum;
    }

    int bufferSizePerline() {
        return _bufferSizePerline;
    }

    int bufferMaxLine() {
        return _bufferMaxLine;
    }

    int mapFileNum() {
        return _mapFileNum;
    }

    int kOfTopK() {
        return _kOfTopK;
    }

};

#endif //HOTURL_CONFIG_H
