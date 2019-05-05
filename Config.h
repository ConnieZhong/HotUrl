//
// Created by conniezhong on 2019/5/2.
//

#ifndef HOTURL_CONFIG_H
#define HOTURL_CONFIG_H

#include <string>
#include "ErrorCode.h"
#include "Comm.h"

using namespace std;

//buffer数量个数的约束
class Config {
private:
    Config() {

    }

    string _inputFilePath;
    string _mapOutFilePrefix;
    string _reduceOutFileName;
    string _fetchOutFileName;

    int _workerThreadNum;
    int _bufferSizePerLine;
    int _bufferMaxLine;
    int _mapFileNum;
    int _kOfTopK;
    int _memTotalByte;
    int _bufferNum;

public:

    int init(string i, string m, string r,string fo, int w,
             int b, int mf, int k, int mm) {
        _inputFilePath = i;
        _mapOutFilePrefix = m;
        _reduceOutFileName = r;
        _fetchOutFileName = fo;

        _workerThreadNum = w;
        _bufferSizePerLine = b;
        _mapFileNum = mf;
        _kOfTopK = k;
        _memTotalByte = mm;


        _bufferNum = _workerThreadNum * _mapFileNum * 2;
        _bufferMaxLine = _memTotalByte / _bufferNum / _bufferSizePerLine;

        if (_bufferMaxLine < 1 || _bufferMaxLine < _kOfTopK) {
            ERROR << "to small memory" << endl;
            return MEMORY_ERROR;
        }
        INFO << "line per buff:" << _bufferMaxLine
             << " buffer num:" << _bufferNum << endl;
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

    string fetchOutFileName(){
        return _fetchOutFileName;
    }

    int workerThreadNum() {
        return _workerThreadNum;
    }

    int bufferSizePerline() {
        return _bufferSizePerLine;
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

    int bufferNum() {
        return _bufferNum;
    }


};

#endif //HOTURL_CONFIG_H
