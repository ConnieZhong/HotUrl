//
// Created by conniezhong on 2019/5/3.
//

#include "File.h"
#include "ErrorCode.h"
#include <sys/types.h>
#include <sys/stat.h>

int File::open(const char *f, int m) {
    if (_fd > 0){
        close(_fd);
    }
    _fileName = f;
    _fd =  ::open(f, m);
    return _fd;
}

int File::read(void *b, unsigned int n) {
    return ::read(_fd, b, n);
}

int File::write(const void *b, unsigned int n) {
    return ::write(_fd, b, n);
}
