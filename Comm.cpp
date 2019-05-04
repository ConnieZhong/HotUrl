//
// Created by conniezhong on 2019/5/3.
//
#include "Comm.h"

unsigned int hotUrlHash(char *str) {
    //BKDRHash
    //TODO 比较一下不同hash
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str) {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

string intToString(int i) {
    char num[256];
    snprintf(num, sizeof(num), "%d", i);
    return num;
}

bool Compare(pair<int, string> a, pair<int, string> b) {
    return a.first > b.first;
}




