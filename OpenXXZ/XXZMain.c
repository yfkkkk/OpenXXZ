//
// Created by yf on 16-8-24.
//
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>

#include "../NetUtil/HttpRequest.h"
#include "XXZGlobal.h"
#include "../CJSON/cJSON.h"


#define BUFSIZE 4096
#define HTTP_RES_HEAD 1024
#define SERVER_ADDR "server.xiaoxinzi.com"
#define SERVER_PATH "/tools/phpsample_yf.php?"
#define PORT "80"

#define true 1
#define false 0


/**
 * request 参数集合
 * @param req
 */
void InitReqData(struct HttpReq* req)
{
    req->srvAddr = SERVER_ADDR;
    req->srvPath = SERVER_PATH;
    req->srvPort = PORT;
    req->params = NULL;
    req->sig = NULL;
    req->gzip = 0;
}

/**
 * api 参数集合
 * @param params
 */
void InitReqParams(XXZReqParams* params)
{
    params->key = "ad9HEIZkblaU9rrmsdt0X0cq";
    params->devid = "or42Zszbf3h0iVk85LzwZyBGo9zs";
    params->user = "100012";
    params->nickname = "小杨";
    params->city = "厦门";
    params->text = NULL;
}

// 各种小信子API
// 输入 param = 参数字符串保存地址
// 输入 apiType = API类型
/**
 * 生成request 需要的api参数字符串
 * @param params api参数列表
 * @param text 对话内容
 * @return 生成的参数字符串
 */
char* CreateReqParamsStr(XXZReqParams* params, char* text)
{
    int len = strlen("key=") + strlen(params->key)
              + strlen("&devid=") + strlen(params->devid)
              + strlen("&user=") + strlen(params->user)
              + strlen("&nickname=") + strlen(params->nickname)
              + strlen("&city=") + strlen(params->city)
              + strlen("&text=") + strlen(text) + 1;
    char* str = malloc(len);
    memset(str, 0, len);
//    printf("字节数:%d\n", len);
    strcat(str, "key=");
    strcat(str, params->key);
    strcat(str, "&devid=");
    strcat(str, params->devid);
    strcat(str, "&user=");
    strcat(str, params->user);
    strcat(str, "&nickname=");
    strcat(str, params->nickname);
    strcat(str, "&city=");
    strcat(str, params->city);
    strcat(str, "&text=");
    strcat(str, text);
    return str;
}

void DeleteReqParamsStr(char* params)
{
    free(params);
}


void GetHttpResponse(int sockfd)
{
    /* 接收HTTP响应消息 */
    int num = 0;
    char        rcvBuf[4096] = {0};
    char       *pRcv         = rcvBuf;
    while(1)
    {
        num = recv(sockfd, pRcv, 4096, 0);
        pRcv += num;

        if((0 == num) || (-1 == num))
        {
            break ;
        }
    }
    /* 打印响应消息 */
//    printf("Received Http Response Content: \n\n%s\n", rcvBuf);
    char *out;cJSON *json;

    json=cJSON_Parse(rcvBuf);
    if (!json)
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else
    {
        int i = cJSON_GetArraySize(json);
        cJSON* jsonTemp = GetJsonValue(json, "data");
        // 打印json对象
        char* out2 = cJSON_Print(jsonTemp);
        printf("%s\n",out2);
        free(out2);
        cJSON_Delete(json);
    }
}

void HttpTalk()
{
    int ret;
    //gzip解压缩
    //printf("zlib版本：%d\n", zlibVersion());
    //网络部分
    int tempfd;
    int sockfd, h;
    struct sockaddr_in servaddr;
    char str1[4096], headBuf[HTTP_RES_HEAD];
    char* str;
    socklen_t len;
    fd_set   t_set1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("创建网络连接失败,本线程即将终止---socket error!\n");
        exit(0);
    };
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    struct hostent* he;
    he = gethostbyname(SERVER_ADDR);
    if(he == NULL)
    {
        /*取得地址信息*/herror("gethostbyname");
        exit(1);
    }
    printf("hostname:%s\n",he->h_name);
    printf("IPAddress:%s\n",inet_ntoa(*((struct in_addr*)he->h_addr)));
    servaddr.sin_addr = *(struct in_addr*)he->h_addr;
//    if (inet_pton(AF_INET, SERVER_ADDR, &servaddr.sin_addr) <= 0)
//    {
//        printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
//        exit(0);
//    };

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("连接到服务器失败,connect error!\n");
        exit(0);
    }
    printf("与远端建立了连接\n");
//    //发送数据
//    memset(str1, 0, 4096);
//    //strcat(str1, "GET  http://server.xiaoxinzi.com/apk/getres3d.php?"
//    strcat(str1, "GET  http://server.xiaoxinzi.com/tools/phpsample_yf.php?"
//            "key=ad9HEIZkblaU9rrmsdt0X0cq"
//            "&devid=or42Zszbf3h0iVk85LzwZyBGo9zs"
//            "&user=100012"
//            "&nickname=小杨"
//            "&city=厦门"
//            "&text=");
//    printf("iat_rec_result:%s\n", iat_rec_result);
//    strcat(str1, iat_rec_result);
//    strcat(str1, " HTTP/1.1\r\n");
//    //strcat(str1, "Host: api.xiaoxinzi.com\r\n");
//    strcat(str1, "Host: server.xiaoxinzi.com\r\n");
//    strcat(str1, "User-Agent: Wget\r\n");
//    strcat(str1, "Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n");
//    strcat(str1, "Accept-Encoding: gzip, deflate, sdch\r\n");
//    strcat(str1, "\r\n");
//    printf("%s\n", str1);
//
//    ret = write(sockfd, str1, strlen(str1));
//    if (ret < 0) {
//        //printf("发送失败！错误代码是%d，错误信息是'%d'\n",errno, errno);//strerror(errno));
//        printf("发送失败！错误代码是%d\n", 11111);
//        exit(0);
//    }
//    else
//        //printf("消息发送成功，共发送了%d个字节！\n\n", ret);
//
//        FD_ZERO(&t_set1);
//    FD_SET(sockfd, &t_set1);

//    // 读取http返回头
//    int contentLength = 0;
//    int p = 0;
//    int headEndP = 0;
//    int res = 0;
//    int reqSize = HTTP_RES_HEAD;
//
//    memset(headBuf, 0, HTTP_RES_HEAD);
//
//    //fixme 假定http头小于 HTTP_RES_HEAD
//    while(1)
//    {
//        res += read(sockfd, headBuf + res, reqSize);
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
//
//    if (res <= 0)
//    {
//        printf("没找到数据！！！\n");
//    }
//    else
//    {
//        // 读取内容长度
//        p = 0;
//        p = http_trans_buf_has_patt(headBuf, res, "Content-Length: ", 16);// 是否有结束标志
//        if(p) {
//            p -= (int)headBuf;
//            int pcnt = p + 16;
//            char contentLengthChar[20];
//            int lengthCount = 0;
//            while (1) {
//                if (headBuf[pcnt] >= 0x30 && headBuf[pcnt] <= 0x39)
//                    contentLengthChar[lengthCount] = headBuf[pcnt];//是数字则写入缓存
//                else
//                    break;//碰到非数字就跳出while循环
//                lengthCount++;
//                pcnt++;
//            }
//            contentLengthChar[lengthCount] = '\0';
//            if (lengthCount != 0)
//                contentLength = atoi(contentLengthChar);
//        }
//    }
//    //取content资料
//    if (contentLength != 0)
//    {
//        //printf("内容总字节数：%d\n", contentLength);
//        char* gzipdata;
//        char* targetdata;
//        char* buf;
//        gzipdata = malloc(contentLength);
//        targetdata = malloc(contentLength * 8);
//        memset(gzipdata, 0, contentLength);
//        memset(targetdata, 0, contentLength * 8);
//        // 取内容起始指针
//        p = headEndP - (int)headBuf;
//        //printf("内容起始指针=%d\n", p);
//        // 截取第一次读取head时，取到的部分内容
//        int p2 = 0;
//        for (p2 = 0; p2 < res - p; p2++)
//        {
//            gzipdata[p2] = headBuf[p + p2];
//        }
//        //printf("已读取内容=%d\n", p2);
//        // 资料是否没有读完
//        if(p2 < contentLength)
//        {
//            while(1)
//            {
//                if(p2 >= contentLength)// 放到开头，防止资料读完了，还去取资料，这样会卡住若干秒。
//                    break;
//                res = read(sockfd, headBuf, HTTP_RES_HEAD);
//                printf("收到资料：%d\n", res);
//                if(res <= 0)
//                    break;
//                else {
//                    memcpy(gzipdata + p2, headBuf, res);
//                    p2 += res;
//                }
//            }
//        }
//        printf("p2=%d\n", p2);
//        int unzipRet = ungzip(gzipdata, contentLength, targetdata, contentLength * 8);
//        printf("unzipRet=%d\n", unzipRet);
//        if (unzipRet  <= 0)
//            printf("解压失败!!!\n");
//        else
//        {
//            printf("解压完成\n");
//            //printf("%s\n", targetdata);
//            memset(targetUtf8, 0, 4096);
//            // json解析
//            char *out;cJSON *json;
//            json=cJSON_Parse(targetdata);
//            char* music = GetData(json, targetUtf8);
//
//            // 用喜马拉雅找音乐
//            cJSON* jsonTemp = GetJsonValue(json, "songname");
//            if(jsonTemp && jsonTemp->valuestring != NULL)
//                music = XMLYGetMusic(jsonTemp->valuestring);
//
//            if(music != NULL)
//            {
//                char* argv[2];
//                argv[1] = music;
//                httpMp3Play(2, argv);
//            }
//            char ccmd[30] = {0};
//            if(GetCcmd(json, ccmd) == 0)
//            {
//                //起线程，发送串口指令
//                printf("json解析到命令：%s\n", ccmd);
//                UART_Operate("/dev/ttyS1", "0", ccmd);
//            }
//            // 显示json结构
//            char* out2 = cJSON_Print(json);
//            printf("%s\n",out2);
//            free(out2);
//            cJSON_Delete(json);
//            printf("json解析完成：%s\n", targetUtf8);
//        }
//        if (gzipdata != NULL)
//        {
//            free(gzipdata);
//            gzipdata = NULL;
//        }
//        if (targetdata != NULL)
//        {
//            free(targetdata);
//            targetdata = NULL;
//        }
//    }
    close(sockfd);
}
static void menu(void)
{
    char* showStr = "dffafasasfefewfwfqefqqfewfqgqgqrqwewq";
    puts("\n");
    puts("+=====================================================================+");
    puts("|                                 小信子                              |");
    puts("|                                    |                                |");
//    printf("| State     : %-12s           | Address: %-21s |\n",
//           showStr, showStr);
    puts("| 功能:                              |                                |");
    puts("| Send Message                       |                                |");
    puts("|    例:> 你好.                      |                                |");
    puts("+------------------------------------+                                |");
    puts("| q  Quit                            |                                |");
    puts("+------------------------------------+--------------------------------+");
    printf(">>> ");
    fflush(stdout);
}


static void console_main(XXZReqParams* params, HttpReq* req)
{
    int q = false;
    menu();
    while (!q)
    {
        char input[32];
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;
        switch (input[0])
        {
            case 's':
                break;
            case 'q':
                q = true;
                break;
            default:
                Talk(params, req, input);
                printf("> ");
        }
    }
}

void Talk(XXZReqParams* params, HttpReq* req, char* text)
{
    int sockfd;
    req->params = CreateReqParamsStr(params, text);// 生成参数字符串
    SendHttpRequest(req, &sockfd, GET);
    free(req->params);
    GetHttpResponse(sockfd);

}

int main(int argc, char *argv[])
{
    XXZReqParams params;
    HttpReq req;

    InitReqData(&req);// 初始化 request 资料
    InitReqParams(&params);// 初始化request参数结构


    console_main(&params, &req);
    return EXIT_SUCCESS;
}