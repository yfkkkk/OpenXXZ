//
// Created by yf on 16-6-28.
//

#ifndef TEST_HTTPREQUEST_H
#define TEST_HTTPREQUEST_H

#endif //TEST_HTTPREQUEST_H

#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>


#include <time.h>
#include <sys/time.h>

#include "../global_enum_struct.h"


#define PORT 80
#define BUFSIZE 4096
#define HTTP_RES_HEAD 1024
enum HttpReqMode
{
    POST,
    GET
};

enum APIFunc
{
    GET_UK,
    UNDSTAND
};


void InitHttpReqStruct(struct HttpReq* req);

int SendHttpRequest(struct HttpReq* httpReqInfo, int* sockfd, enum HttpReqMode mode);

int tcp_connect(const char* host, const char* serv);

//int GetHttpResponse(int sockfd);