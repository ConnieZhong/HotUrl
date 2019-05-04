//
// Created by conniezhong on 2019/5/3.
//

#include <cstring>
#include "BufferManager.h"
#include "Comm.h"
#include "ErrorCode.h"
#include "Config.h"

int FileBuffer::initFile(string fileName, int mod) {
    File tmp;
    _file = tmp;
    int ret = _file.open(fileName.c_str(), mod);
    if (ret <= 0) {
        ERROR << "file buffer open file err, ret:" << ret << " file:" << fileName << endl;
        _status = ret;
        return _status;
    }
    _status = SUCCESS;
    return _status;
}

int BaseBuffer::init(int spl, int ln) {
    if (spl <= 0 || ln <= 0) {
        ERROR << "file buffer parm err" << endl;
        _status = PARM_ERROR;
        return _status;
    }
    _sizeCapacityPerLine = spl;
    _lineNumCapacity = ln;
    _lineNow = 0;
    _data = make_shared_array<char>(_sizeCapacityPerLine * _lineNumCapacity);
    if (_data.get() == NULL) {
        ERROR << "file buffer alloc error" << endl;
        _status = MEMORY_ERROR;
        return _status;
    }

    void *tmp = memset(_data.get(), _sizeCapacityPerLine * _lineNumCapacity, 0);
    if (tmp == NULL) {
        ERROR << "file buffer memset error" << endl;
        _status = MEMORY_ERROR;
        return _status;
    }
    DEBUG << "addr:" << (long) (_data.get()) << "==========" << _sizeCapacityPerLine << ":" << _lineNumCapacity << endl;
    _status = SUCCESS;
    return _status;
}


int BaseBuffer::clear() {
    _lineNow = 0;
    if (_data.get() != NULL) {
        void *ret = memset(_data.get(),
                           _sizeCapacityPerLine * _lineNumCapacity, 0);
        return ret != NULL ? SUCCESS : MEMORY_ERROR;
    }
    return MEMORY_ERROR;
}

int BaseBuffer::addLine(char *data) {
    if (_lineNow == _lineNumCapacity - 1) {
        ERROR << "buff full" << endl;
        return BUFFER_FULL;
    }
    DEBUG << "addr" << (long) (_data.get()) << " line now:" << _lineNow << " sizecapperLine:" << _sizeCapacityPerLine
          << " :"
          << _lineNumCapacity << endl;
    void *ret = memcpy(_data.get() + _lineNow * _sizeCapacityPerLine, data, _sizeCapacityPerLine);
    if (ret == NULL) {
        ERROR << "add line err." << endl;
        return MEMORY_ERROR;
    }
    _lineNow++;
    return SUCCESS;
}

int BaseBuffer::addLine(const char *data, int num) {
    if (_lineNow == _lineNumCapacity - 1) {
        ERROR << "buff full" << endl;
        return BUFFER_FULL;
    }
    void *ret = memcpy(_data.get() + _lineNow * _sizeCapacityPerLine, data, _sizeCapacityPerLine);
    if (ret == NULL) {
        ERROR << "add line err." << endl;
        return MEMORY_ERROR;
    }
    ret = memcpy(_data.get() + (_lineNow + 1) * _sizeCapacityPerLine - 4, &num, sizeof(int));
    if (ret == NULL) {
        ERROR << "add line err." << endl;
        return MEMORY_ERROR;
    }

    _lineNow++;
    return SUCCESS;
}

int BaseBuffer::getLine(int lineNo, char **out) {
    if (lineNo >= _lineNumCapacity) {
        ERROR << "get line num parm err. line no:" << lineNo << endl;
        return PARM_ERROR;
    }
    *out = _data.get() + lineNo * _sizeCapacityPerLine;
    return SUCCESS;
}

int BaseBuffer::getLine(int lineNo, char **out, int &num) {
    if (lineNo >= _lineNumCapacity) {
        ERROR << "get line num parm err. line no:" << lineNo << endl;
        return PARM_ERROR;
    }
    *out = _data.get() + lineNo * _sizeCapacityPerLine;
    num = *(int *) (_data.get() + (lineNo + 1) * _sizeCapacityPerLine - 4);
    return SUCCESS;
}

bool BaseBuffer::isFull() {
    return _lineNow == _lineNumCapacity;
}

bool BaseBuffer::isEmpty() {
    return _lineNow == 0;
}

int FileBuffer::writeToFile() {

    int ret = _file.write(_data.get(), _sizeCapacityPerLine * _lineNow);
    if (ret == _sizeCapacityPerLine * _lineNow) {
        return SUCCESS;
    }
    return ret;
}

//TODO 修改参数
int FileBuffer::readFromFile() {

    int ret = _file.read(_data.get(), _sizeCapacityPerLine * _lineNumCapacity);
    if (ret % _sizeCapacityPerLine != 0) {
        ERROR << "size err. ret:" << ret << endl;
        return INNER_ERROR;
    }

    if (ret < 0) {
        ERROR << "read file err. ret:" << ret << endl;
        return ret;
    }
    _lineNow = ret / _sizeCapacityPerLine;
    return ret;
}

int BufferManager::init(int totalBuffer) {
    _totalBufferNum = totalBuffer;
    //事先分配好内存
    for (int i = 0; i < _totalBufferNum; ++i) {
        shared_ptr<FileBuffer> buffer = make_shared<FileBuffer>();
        if (buffer.get() == NULL) {
            ERROR << "buffer init err" << endl;
            return MEMORY_ERROR;
        }
        int ret = buffer->init(Config::getInstance().bufferSizePerline(),
                               Config::getInstance().bufferMaxLine());
        if (ret != SUCCESS) {
            ERROR << "buffer init err. ret:" << ret << endl;
            return ret;
        }
        _que.push(buffer);
    }
    return SUCCESS;
}






