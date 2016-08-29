//
// Created by yf on 16-6-28.
//

#include "HttpRequest.h"
#define BACK_LOG 3
// 初始化结构
void InitHttpReqStruct(struct HttpReq* req)
{
    req->srvAddr = 0;
    req->srvPath = 80;//默认http请求
    memset(req->params, 0, 1024);
    memset(req->sig, 0, 40);
}

int tcp_listen(const char* host, const char* serv)
{
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        printf("tcp_listen error for %s, %s: %s\n",
               host, serv, gai_strerror(n));
        return -1;
    }

    ressave = res;
    do {
        listenfd = socket(res->ai_family, res->ai_socktype,
                          res->ai_protocol);
        if(listenfd < 0) {
            continue;
        }

        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if(bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }

        close(listenfd);
    }while((res = res->ai_next) != NULL);

    if(res == NULL) {
        printf("tcp_listen error for %s, %s: %s\n",
               host, serv, gai_strerror(n));
        return -1;
    }

    listen(listenfd, BACK_LOG);

    freeaddrinfo(ressave);
    return(listenfd);
}

int tcp_connect(const char* host, const char* serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;      /* Allow IPv4 */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_socktype = SOCK_STREAM;
//    printf("host:%s\n", host);
//    printf("serv:%s\n", serv);
    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {

        printf("tcp_connect error for %s, %s: %s\n",
               host, serv, gai_strerror(n));
        return -1;
    }
    ressave = res;
    do
    {
        sockfd = socket(res->ai_family, res->ai_socktype,
                        res->ai_protocol);
        if (sockfd < 0)
        {
            continue;
        }
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
        {
            break;
        }

        close(sockfd);
    } while ((res = res->ai_next) != NULL);
    if (res == NULL)
    {
        printf("tcp_connect error for %s, %s: %s\n",
               host, serv, gai_strerror(n));
        return -1;
    }

    freeaddrinfo(ressave);
    return (sockfd);
}

int SendHttpRequest(struct HttpReq* httpReqInfo, int* sockfd, enum HttpReqMode mode)
{
    int ret;
    unsigned char str1[4096];
    fd_set   t_set1;

    *sockfd = tcp_connect(httpReqInfo->srvAddr, httpReqInfo->srvPort);

    //发送数据
    memset(str1, 0, 4096);
    if(mode == POST)
        strcat(str1, "POST  http://");
    else
        strcat(str1, "GET  http://");
    strcat(str1, httpReqInfo->srvAddr);
    strcat(str1, httpReqInfo->srvPath);
    strcat(str1, httpReqInfo->params);
//    if(httpReqInfo->sig != NULL)
//    {
//        strcat(str1, "&sig=");
//        strcat(str1, httpReqInfo->sig);
//    }
    strcat(str1, "\r\nHTTP/1.1\r\n");
    strcat(str1, "Host: ");
    strcat(str1, httpReqInfo->srvAddr);
    strcat(str1, "\r\nUser-Agent: Wget\r\n");
    strcat(str1, "Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n");
    if(httpReqInfo->gzip == 1)//response有gzip加密
        strcat(str1, "Accept-Encoding: gzip, deflate, sdch\r\n");
    strcat(str1, "\r\n");

//    printf("%s\n", str1);

    ret = write(*sockfd, str1, strlen(str1));
    if (ret < 0) {
        printf("发送失败！错误代码是%d，错误信息是'%d'\n",errno, errno);//strerror(errno));
        exit(0);
    }
//    else
//        printf("消息发送成功，共发送了%d个字节！\n\n", ret);

    FD_ZERO(&t_set1);
    FD_SET(*sockfd, &t_set1);
}

char* http_trans_buf_has_patt(char *a_buf, int a_len, char *a_pat, int a_patlen) {
    int i = 0;
    for (; i <= (a_len - a_patlen); i++) {
        if (a_buf[i] == a_pat[0]) {
            if (memcmp(&a_buf[i], a_pat, a_patlen) == 0)
                return &a_buf[i];
        }
    }
    return NULL;
}

//int GetHttpResponse(int sockfd)
//{
//    // 读取http返回头
//    int contentLength = 0;
//    int p = 0;
//    int headEndP = 0;
//    int res = 0;
//    int reqSize = HTTP_RES_HEAD;
//    char headBuf[HTTP_RES_HEAD];
//
//    memset(headBuf, 0, HTTP_RES_HEAD);
//
//    //fixme 假定http头小于 HTTP_RES_HEAD
//    while(1)
//    {
//        res += read(sockfd, headBuf + res, reqSize);
//        //res += recv(sockfd, headBuf + res, reqSize, 0);
//        printf("head收到资料：%d\n", res);
//        if (res <= 0)// 出错了
//            break;
//        headEndP = http_trans_buf_has_patt(headBuf, res, "\r\n\r\n", 4);// 是否有结束标志
//        if(headEndP != 0) {
//            headEndP += 4;
//            *(char*)(headEndP-1) = '\0';
//            printf("%s", headBuf);
//            printf("指向：%d\n", (int)headEndP - (int)headBuf);
//            break;
//        }
//        else// 没找到结束标志，继续循环读取http资料
//        {
//            if(res < HTTP_RES_HEAD)
//                reqSize = HTTP_RES_HEAD - res;// 缓存未满，继续读取
//            else{
//                printf("溢出：超出head缓存！！！");
//                break;
//            }
//        }
//    }
//}

