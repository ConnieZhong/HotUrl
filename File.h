//
// Created by conniezhong on 2019/5/3.
//

#ifndef HOTURL_FILE_H
#define HOTURL_FILE_H


#include <fcntl.h>
#include <string>
#include <unistd.h>

using namespace std;
class File {
private:
    int _fd;
    string _fileName;
public:
    int open(const char *, int, int );

    int read(void *, unsigned int);

    int write(const void *, unsigned int);

    File() : _fd(0) {
    }

    ~File() {
        if (_fd != 0) {
            close(_fd);
        }
    }

    string fileName(){
        return _fileName;
    }

};

#endif //HOTURL_FILE_H
