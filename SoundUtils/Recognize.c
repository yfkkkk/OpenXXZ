#include "SoundUtils.h"
#include "../Debug/Debug.h"
//讯飞
#include "../Ifly/qisr.h"
#include "../Ifly/qtts.h"
#include "../Ifly/msp_cmn.h"
#include "../Ifly/msp_errors.h"
#include "../Ifly/msp_types.h"

#define	BUFFER_SIZE	4096
#define FRAME_LEN	640
#define HINTS_SIZE  100

extern void sysUsecTime();
////////////////////////////////////
//讯飞初始化变量
////////////////////////////////////
// widora
const char* login_params = "appid = 56c6bdb5, work_dir = .";
// linux
//const char* login_params = "appid = 56ce5efa, work_dir = .";

int			upload_on = 0; //是否上传用户词表
const char* session_begin_params = "sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";
int ret = MSP_SUCCESS;

int iflyLogin()
{
    int ret;
    /* 用户登录 */
    ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数
    if (MSP_SUCCESS != ret)
    {
        printf("讯飞登录失败，即将登出，错误码：%d.\n", ret);
        MSPLogout();
        return -1; //登录失败，退出登录
    }
    printf("成功登录讯飞\n");
    return 0;//正确返回
}

/**
 * @brief 功能：讯飞语音识别
 */
extern void RecognizeVoice_Ifly(char* recgResult)
{
    const char *session_id = NULL;

    char hints[HINTS_SIZE] = {NULL}; //hints为结束本次会话的原因描述，由用户自定义
    unsigned int total_len = 0;
    int aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;        //音频状态
    int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;        //端点检测
    int rec_stat = MSP_REC_STATUS_SUCCESS;            //识别状态
    int errcode = MSP_SUCCESS;

    FILE *f_pcm = NULL;
    char *p_pcm = NULL;
    long pcm_count = 0;
    long pcm_size = 0;
    long read_size = 0;

    char windNoise[1024] = {0};
    int windNoiseP = 0;
    char meetSpeech = 0;
    sysUsecTime();
    iflyLogin();
    printf("\n开始语音听写 ...\n");
    session_id = QISRSessionBegin(NULL, session_begin_params, &errcode); //听写不需要语法，第一个参数为NULL
    if (MSP_SUCCESS != errcode)
    {
        printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
        goto iat_exit;
    }
    int switcher = 0;// 缓存切换用
    int t = 0;
    while (1)
    {
        if (G_sndC.onPcmBufState[switcher] == BufState_Full)
        {
            _printf("————开始识别\n");
            G_sndC.onPcmBufState[switcher] = BufState_Locked;// 锁定
            ////////////////////////////////////////////
            // 读缓存并处理
            printf("发送识别!\n");
            ret = QISRAudioWrite(session_id, G_sndC.pcmBuf[switcher], G_sndC.pcmBufSize, aud_stat, &ep_stat,
                                 &rec_stat);
            if (MSP_SUCCESS != ret)
            {
                printf("\nQISRAudioWrite failed! error code:%d\n", ret);
                goto iat_exit;
            }
            if (MSP_EP_AFTER_SPEECH == ep_stat)
                break;
            ////////////////////////////////////////////
            G_sndC.onPcmBufState[switcher] = BufState_Empty;// 读完，置空标志
            switcher = !switcher;
            _printf("————切换到缓存%d\n", switcher);
            t++;
            if (t >= 3)
            {
                _printf("————要终止录音了！\n");
                G_sndC.onRecord = 0;
                //G_sndC.onKillAllThread = 1;
                break;
            } else
            {
                _printf("————  次数：%d\n", t);
            }
        } else
        {
            _printf("————暂停%d\n", switcher);
            usleep(200 * 1000);
        }
    }

    // 取结果
    sysUsecTime();
    errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
    if (MSP_SUCCESS != errcode)
    {
        printf("\nQISRAudioWrite failed! error code:%d \n", errcode);
        return;
    }
    while (MSP_REC_STATUS_COMPLETE != rec_stat)
    {
        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
        if (MSP_SUCCESS != errcode)
        {
            printf("\nQISRGetResult failed, error code: %d\n", errcode);
            return;
        }

        if (NULL != rslt)
        {
            unsigned int rslt_len = strlen(rslt);
            total_len += rslt_len;
            if (total_len >= BUFFER_SIZE)
            {
                printf("\nno enough buffer for rec_result !\n");
                return;
            }
            strncat(recgResult, rslt, rslt_len);
        }
        //usleep(150 * 1000); //防止频繁占用CPU
    }
    printf("\n语音听写结束\n");
    printf("=============================================================\n");
    printf("%s\n", recgResult);
    printf("=============================================================\n");
    sysUsecTime();
    iat_exit:
    return;

}