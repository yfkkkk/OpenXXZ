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
#define SERVER_GET_UK "get.xiaoxinzi.com"
#define SERVER_GET_UK_PATH "/app_event.php"
#define SERVER_UNDSTAND "ai.xiaoxinzi.com" //"server.xiaoxinzi.com"
#define SERVER_UNDSTAND_PATH "/api3.php" //"/tools/phpsample_yf.php?"
#define PORT "80"

#define true 1
#define false 0

/////////////////////////////////////
// 外部引入接口
/////////////////////////////////////
extern struct SoundCtrl G_sndC;
extern void RecordDeviceInit();
extern void Record();
extern int IflyLogin();
extern void RecognizeVoice_Ifly(char* recgResult);

/**
 * request 参数集合
 * @param req
 */
void InitReqData(struct HttpReq* req, enum APIFunc apiFunc)
{
    if(apiFunc == GET_UK)
    {
        req->srvAddr = SERVER_GET_UK;
        req->srvPath = SERVER_GET_UK_PATH;
    } else if(apiFunc == UNDSTAND)
    {
        req->srvAddr = SERVER_UNDSTAND;
        req->srvPath = SERVER_UNDSTAND_PATH;
    }
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
    params->appid = "dcXbXX0X";
    params->ak = "5c011b2726e0adb52f98d6a57672774314c540a0";
    params->token = "f9e79b0d9144b9b47f3072359c0dfa75926a5013";
    params->devid = "UniqueDeviceID";
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
char* CreateReqParamsStr(XXZReqParams* params, char* text, enum APIFunc apiFunc)
{
    if(apiFunc == GET_UK)
    {
        int len = strlen("secret=") + strlen(params->appid) + 1
                  + strlen(params->ak) + 1
                  + strlen(params->token)
                  + strlen("&event=GetUk")
                  + strlen("&data=") + strlen(params->user) + 2 + 2 + 1;// [\"  \"]
        char* str = malloc(len);
        memset(str, 0, len);
//    printf("字节数:%d\n", len);
        strcat(str, "secret=");
        strcat(str, params->appid);
        strcat(str, "|");
        strcat(str, params->ak);
        strcat(str, "|");
        strcat(str, params->token);
        strcat(str, "&event=GetUk");
        strcat(str, "&data=[\"");
        strcat(str, params->devid);
        strcat(str, "\"]");
        return str;
    } else if(apiFunc == UNDSTAND)
    {
        if(params->uk == NULL)
        {
            _error("uk 不存在, 请先获取uk\n");
            return NULL;
        }
        int len = strlen("app=") + strlen(params->appid)
                  + strlen("&dev=") + strlen(params->devid)
                  + strlen("&uk=") + strlen(params->uk)
                  + strlen("&text=") + strlen(text) + 1;
        char* str = malloc(len);
        memset(str, 0, len);
    //    printf("字节数:%d\n", len);
        strcat(str, "app=");
        strcat(str, params->appid);
        strcat(str, "&dev=");
        strcat(str, params->devid);
        strcat(str, "&uk=");
        strcat(str, params->uk);
        strcat(str, "&text=");
        strcat(str, text);
        return str;
    }
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

cJSON* GetHttpResponse(int sockfd)
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
    //printf("Received Http Response Content: \n\n%s\n", rcvBuf);
    int i = 0;
    char* dataP = 0;// 指向资料位置
    while(rcvBuf[i] != 0)
    {
        if(rcvBuf[i] == '\r')
        {
            if(!strncasecmp(rcvBuf + i, "\r\n\r\n", 4))
            {
                dataP = rcvBuf + i + 4;
                break;
            }
        }
        i++;
    }

    char *out;cJSON *json;

    json = cJSON_Parse(dataP);
    return json;
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
    puts("| u  GetUK                           |                                |");
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
            case 'u':
                GetUK(params, req);
                printf("> ");
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

/**
 * 文本小信子, 发送对话请求
 * @param params 请求参数
 * @param req 请求
 * @param text 语句
 */
void Talk(XXZReqParams* params, HttpReq* req, char* text)
{
    int sockfd;

    InitReqData(req, UNDSTAND);// 初始化 request 资料
    req->params = CreateReqParamsStr(params, text, UNDSTAND);// 生成参数字符串
    if(req->params == NULL)
    {
        _error("无法生成参数字符串!\n");
        return;
    }
    SendHttpRequest(req, &sockfd, POST);
    free(req->params);
    cJSON* json = GetHttpResponse(sockfd);
    if (!json)
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else
    {
        ShowResponse(json);
    }
}

void GetUK(XXZReqParams* params, HttpReq* req)
{
    int sockfd;

    InitReqData(req, GET_UK);// 初始化 request 资料
    req->params = CreateReqParamsStr(params, NULL, GET_UK);// 生成参数字符串
    SendHttpRequest(req, &sockfd, POST);
    free(req->params);
    cJSON* json = GetHttpResponse(sockfd);
    if (!json)
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else
    {
        //ShowResponse(json);
//        // 打印json对象
//        char* out2 = cJSON_Print(json);
//        printf("%s\n",out2);
//        free(out2);
//        cJSON_Delete(json);
        params->uk = malloc(41);
        memset(params->uk, 0, 41);
        memcpy(params->uk, GetJsonValue(json, "uk")->valuestring, 40);
    }
}

//int main(int argc, char *argv[])
//{
//
//    char iat_rec_result[4096] = {0};
//
//    XXZReqParams params;
//    HttpReq req;
//
//    InitReqData(&req);// 初始化 request 资料
//    InitReqParams(&params);// 初始化request参数结构
////
////
////    console_main(&params, &req);
//    G_sndC.onRecord = 1;
//    RecordDeviceInit();
//    int thrRes;//创建的线程返回值
//    thrRes = pthread_create(&G_sndC.recordThread, NULL, Record, "test.wav");
//    if (thrRes != 0)
//    {
//        _error("Thread creation failed!");
//        exit(EXIT_FAILURE);
//    }
//    IflyLogin();// 登陆讯飞
//    while(1)
//    {
//        memset(iat_rec_result, 0, 4096);
//        RecognizeVoice_Ifly(iat_rec_result);
//        Talk(&params, &req, iat_rec_result);
//    }
//    return EXIT_SUCCESS;
//}

// 文本测试main函数
int main(int argc, char *argv[])
{

    char iat_rec_result[4096] = {0};

    XXZReqParams params;
    HttpReq req;
    InitReqParams(&params);// 初始化request参数结构
    console_main(&params, &req);
}