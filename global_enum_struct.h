//
// Created by yf on 16-6-29.
//

#ifndef TEST_GLOBAL_ENUM_H
#define TEST_GLOBAL_ENUM_H

//struct HttpReq
//{
//    char* ipstr;
//    char* path;
//    int port;
//    char params[1024];
//    char sig[40];
//};

typedef struct HttpReq
{
    char* srvAddr;      // 域名或地址字符串
    char* srvPath;      // 路径字符串
    char* srvPort;      // 端口字符串
    char* params;       // 全部参数字符串
    char* sig;          // 计算出的标签字符串
    char gzip;          // response是否有gzip加密
}HttpReq;

#endif //TEST_GLOBAL_ENUM_H

