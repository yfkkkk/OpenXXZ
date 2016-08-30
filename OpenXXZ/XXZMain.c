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

#include "../SoundUtils/SoundUtils.h"
#include "../Debug/Debug.h"

#define BUFSIZE 4096
#define HTTP_RES_HEAD 1024
#define SERVER_ADDR "server.xiaoxinzi.com"
#define SERVER_PATH "/tools/phpsample_yf.php?"
#define PORT "80"

#define true 1
#define false 0

/////////////////////////////////////
// 外部引入接口
/////////////////////////////////////
extern struct SoundCtrl G_sndC;
extern void RecordDeviceInit();
extern void Record();
extern int iflyLogin();
extern void RecognizeVoice_Ifly(char* recgResult);

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

void ShowResponse(cJSON* json)
{
    cJSON* jsonP = GetJsonValue(json, "datatype");
    printf("datatype: %s\n", jsonP->valuestring);
    if(!strcmp(jsonP->valuestring, "text"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }

        jsonP = GetJsonValue(json, "data");
        if(jsonP != NULL)
        {
            data = jsonP->valuestring;
            printf("%s\n", data);
        }
    }
    else if(!strcmp(jsonP->valuestring, "weather"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }
        jsonP = GetJsonValue(json, "city");
        printf("city: %s\n", jsonP->valuestring);
        jsonP = (GetJsonValue(json, "data")->child)->child;
        printf("date: %s\n", jsonP->valuestring);
        jsonP = jsonP->next;
        printf("temperature: %s\n", jsonP->valuestring);
        jsonP = jsonP->next;
        printf("weather: %s\n", jsonP->valuestring);
        jsonP = jsonP->next;
        printf("wind: %s\n", jsonP->valuestring);
        jsonP = jsonP->next;
        printf("dayPictureUrl: %s\n", jsonP->valuestring);
    }
    else if(!strcmp(jsonP->valuestring, "news"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }
        jsonP = GetJsonValue(json, "data")->child;
        while(jsonP != NULL)
        {
            printf("title: %s\n", jsonP->child->valuestring);
            printf("link: %s\n", jsonP->child->next->valuestring);
            jsonP = jsonP->next;
        }
//        cJSON* jsonTmpP;
//        while(jsonP != NULL)
//        {
//            jsonTmpP = jsonP->child;
//            printf("title: %s\n", jsonTmpP->valuestring);
//            jsonTmpP = jsonTmpP->next;
//            printf("price: %s\n", jsonTmpP->valuestring);
//            jsonTmpP = jsonTmpP->next;
//            printf("picurl: %s\n", jsonTmpP->valuestring);
//            jsonTmpP = jsonTmpP->next;
//            printf("link: %s\n", jsonTmpP->valuestring);
//            jsonP = jsonP->next;
//        }
    }
    else if(!strcmp(jsonP->valuestring, "shop"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }
        jsonP = GetJsonValue(json, "data")->child;
        cJSON* jsonTmpP;
        while(jsonP != NULL)
        {
            jsonTmpP = jsonP->child;
            printf("title: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("decsription: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("picurl: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("link: %s\n", jsonTmpP->valuestring);
            jsonP = jsonP->next;
        }
    }
    else if(!strcmp(jsonP->valuestring, "path"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }
        jsonP = GetJsonValue(json, "data")->child;
        cJSON* jsonTmpP;
        while(jsonP != NULL)
        {
            jsonTmpP = jsonP->child;
            printf("title: %s\n", jsonTmpP->valuestring);
            jsonP = jsonP->next;
        }
    }
    else if(!strcmp(jsonP->valuestring, "phone"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }

        jsonP = GetJsonValue(json, "data");
        if(jsonP != NULL)
        {
            data = jsonP->valuestring;
            printf("%s\n", data);
        }
    }
    else if(!strcmp(jsonP->valuestring, "music"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }
        jsonP = GetJsonValue(json, "data")->child;
        cJSON* jsonTmpP;
        while(jsonP != NULL)
        {
            jsonTmpP = jsonP->child;
            printf("title: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("artistName: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("link: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("picurl: %s\n", jsonTmpP->valuestring);
            jsonP = jsonP->next;
        }
    }
    else if(!strcmp(jsonP->valuestring, "webShop"))
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }
        jsonP = GetJsonValue(json, "data")->child;
        cJSON* jsonTmpP;
        while(jsonP != NULL)
        {
            jsonTmpP = jsonP->child;
            printf("title: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("link: %s\n", jsonTmpP->valuestring);
            jsonTmpP = jsonTmpP->next;
            printf("picurl: %s\n", jsonTmpP->valuestring);
            jsonP = jsonP->next;
        }
    }
    else
    {
        char* text;
        char* data;
        jsonP = GetJsonValue(json, "text");
        if(jsonP != NULL)
        {
            text = jsonP->valuestring;
            if(text[0] != 0)
                printf("%s\n", text);
        }

        jsonP = GetJsonValue(json, "data");
        if(jsonP != NULL)
        {
            data = jsonP->valuestring;
            printf("%s\n", data);
        }
    }
}

void GetHttpResponse(int sockfd)
{
    /* 接收HTTP响应消息 */
    int num = 0;
    char        rcvBuf[40960] = {0};
    char       *pRcv         = rcvBuf;
    while(1)
    {
        num = recv(sockfd, pRcv, 40960, 0);
        pRcv += num;

        if((0 == num) || (-1 == num))
        {
            break;
        }
    }
    /* 打印响应消息 */
    printf("Received Http Response Content: \n\n%s\n", rcvBuf);
    char *out;cJSON *json;

    json = cJSON_Parse(rcvBuf);
    if (!json)
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else
    {
        ShowResponse(json);
//        // 打印json对象
//        char* out2 = cJSON_Print(jsonTemp);
//        printf("%s\n",out2);
//        free(out2);
        cJSON_Delete(json);
    }
}

static void menu(void)
{
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



int Rec()
{
    //登录讯飞
    if (iflyLogin())
    {
        return -1;
    }
    G_sndC.onRecord = 1;
    RecordDeviceInit();
    int thrRes;//创建的线程返回值
    thrRes = pthread_create(&G_sndC.recordThread, NULL, Record, NULL);
    if (thrRes != 0)
    {
        _error("————Thread creation failed!");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * 文本小信子
 * @param params
 * @param req
 * @param text
 */
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

    char iat_rec_result[4096] = {0};

    XXZReqParams params;
    HttpReq req;

    InitReqData(&req);// 初始化 request 资料
    InitReqParams(&params);// 初始化request参数结构
//
//
//    console_main(&params, &req);
    G_sndC.onRecord = 1;
    RecordDeviceInit();
    int thrRes;//创建的线程返回值
    thrRes = pthread_create(&G_sndC.recordThread, NULL, Record, "test.wav");
    if (thrRes != 0)
    {
        _error("Thread creation failed!");
        exit(EXIT_FAILURE);
    }
    RecognizeVoice_Ifly(iat_rec_result);

    Talk(&params, &req, iat_rec_result);
    return EXIT_SUCCESS;
}