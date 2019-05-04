#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

const int PARM_ERROR = -1;

string getRandStrByLength(const int len) {
    char *str = new char[len];
    int i;
    for (i = 0; i < len; ++i) {
        str[i] = 'A' + rand() % 26;
    }
    str[++i] = '\0';
    string tmp = str;
    delete (str);
    return tmp;
}

int getRandomNumByRange(int a, int b) {
    srand(time(NULL));
    return rand() % (a - b + 1) + b;
}

struct FileInfo {
    int fileNo = 0;
    int minLine = 0;
    int maxLine = 0;
    int urlLen = 0;
    string filePath;
};

string intToString(int i) {
    char num[256];
    snprintf(num, sizeof(num), "%d", i);
    return num;
}

int main(int argc, char *argv[]) {
    string urlPrefix = "http://";
    string urlSuffix = ".com";

    //TODO �ḻʹ��˵�� --helpѡ��
    //����1:���ٸ��ļ�
    //����2:ÿ���ļ����ٶ�����url
    //����3��ÿ���ļ���������url
    //����3��ÿ��url���ٳ��ȵ�url
    //����4���������λ��
    //TODO �ḻ����������
    int urlMinLength = urlPrefix.length() + urlSuffix.length() + 1;
    string help = "parm should be: num-of-file min-line-of-file "
                  "max-line-of-file length-of-file(should be bigger than " + intToString(urlMinLength) +
                  ") out-put-path";
    if (argc != 6) {
        cout << "parm err, " << help << endl;
        return PARM_ERROR;
    }


    FileInfo fileInfo;
    fileInfo.fileNo = atoi(argv[1]);
    fileInfo.minLine = atoi(argv[2]);
    fileInfo.maxLine = atoi(argv[3]);
    fileInfo.urlLen = atoi(argv[4]) - urlPrefix.length() - urlSuffix.length();
    if (fileInfo.urlLen <= 0) {
        cout << "parm err, " << help << endl;
        return PARM_ERROR;
    }
    fileInfo.filePath = argv[5];

    //�����ļ���
    int ret = mkdir(fileInfo.filePath.c_str(), 0777);
    cout << "begin generate file..." << endl;
    for (int fileIndex = 0; fileIndex < fileInfo.fileNo; ++fileIndex) {
        ofstream file; //����������
        file.open(fileInfo.filePath + "/" + intToString(fileIndex)); //�����ļ�
        int line = getRandomNumByRange(fileInfo.minLine, fileInfo.maxLine);
        for (int lineIndex = 0; lineIndex < line; ++lineIndex) {
            string url = urlPrefix + getRandStrByLength(fileInfo.urlLen) + urlSuffix;
            file << url << endl;
        }
        file.close();
    }
    cout << "generate file over" << endl;
    return 0;
}