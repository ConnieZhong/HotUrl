#include "ReduceTask.h"
#include "Comm.h"
#include "Config.h"
#include "Scheduler.h"
#include "File.h"
#include "BufferManager.h"
#include <unordered_map>
#include <cstring>

//
// Created by conniezhong on 2019/5/3.
//

void ReduceTask::run() {
    //ͳ�Ʋ�ͬ��url���ֵĴ������ڴ�
    //TODO ��ʼ���Ż�
    unordered_map<string, int> hashMap;
    shared_ptr<FileBuffer> fileBuffer;
    BufferManager::getInstance().getBuffer(fileBuffer);
    int ret = fileBuffer->initFile(_mapFileName, O_RDONLY);
    if (ret != SUCCESS) {
        ERROR << "file buffer init err. ret:" << ret << " file name:" << _mapFileName << endl;
        return;
    }

    //������ȡ
    while (true) {
        ret = fileBuffer->readFromFile();
        if (ret < 0) {
            ERROR << "read file err. ret:" << ret << " file name:" << _mapFileName << endl;
            return;
        }

        if (ret == 0) {
            INFO << "file read over. file name:" << _mapFileName;
            break;
        }
        int num = fileBuffer->lineNow();
        for (int i = 0; i < num; ++i) {
            char *data;
            ret = fileBuffer->getLine(i, &data);
            if (ret != SUCCESS) {
                ERROR << "read buffer err. ret:" << ret << " file name:" << _mapFileName << endl;
                return;
            }
            hashMap[data] += 1;
        }
    }

    for (auto it : hashMap) {
        DEBUG << "map file name: " << _mapFileName << " hash map is:" << it.first << ":" << it.second << endl;
    }

    //��hashmapȡ����������100��,д���ļ�����
    //����ĸ��ֽڴ�Ŵ���

    fileBuffer->clear();
    fileBuffer->initFile(Config::getInstance().reduceOutFileName(), O_APPEND | O_CREAT | O_WRONLY);

    vector<pair<int, string>> vec(Config::getInstance().kOfTopK(), make_pair(0, ""));
    make_heap(vec.begin(), vec.end(), Compare);
    for (auto it = hashMap.begin(); it != hashMap.end(); it++) {
        if (it->second > vec[0].first) {
            vec[0].first = it->second;
            vec[0].second = it->first;
            sort_heap(vec.begin(), vec.end());
        }
    }

    //TODO ȥ��Σ�պ���
    for (size_t i = 0; i < vec.size(); ++i) {
        ret = fileBuffer->addLine(vec[i].second.c_str(), vec[i].first);
        if (ret != SUCCESS) {
            ERROR << "copy num error, file:" << _mapFileName << endl;
            return;;
        }
    }
    ret = fileBuffer->writeToFile();
    if (ret != SUCCESS) {
        ERROR << "write file error, file:" << _mapFileName << endl;
        return;;
    }

    Scheduler::getInstance().reportReduceTaskFinishedOne();
}
