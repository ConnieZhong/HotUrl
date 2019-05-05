//
// Created by conniezhong on 2019/5/3.
//

#ifndef HOTURL_COMM_H
#define HOTURL_COMM_H

#include <iostream>
#include <string>
#include <memory>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "TaskManager.h"


using namespace std;

unsigned int hotUrlHash(char *str);

string intToString(int i);

bool Compare(pair<int, string> a, pair<int, string> b);
template <typename T>
shared_ptr<T> make_shared_array(size_t size)
{
    return shared_ptr<T>(new T[size], default_delete<T[]>());
}

void doSomeThing(shared_ptr<BaseTask> ptr);

#define MODE S_IRWXU | S_IXGRP | S_IROTH | S_IXOTH

#define INFO std::cout << "INFO|" << __FILE__ << ":" << __LINE__ << " "
#define ERROR std::cout << "ERROR|" << __FILE__ << ":" << __LINE__ << " "
#define DEBUG  std::cout << "DEBUG|" << __FILE__ << ":" << __LINE__ << " "
#endif //HOTURL_COMM_H
