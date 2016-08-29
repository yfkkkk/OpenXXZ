//
// Created by yf on 16-7-13.
//

# include <pthread.h>
# include "../Debug/Debug.h"
# include "SoundUtils.h"

# define PCM_BUF_SIZE 16000 //16k，跟识别单次发送字节一致

/////////////////////////////////////
// 外部引入接口
/////////////////////////////////////
extern void sysUsecTime();//引用

/////////////////////////////////////
// 结构初始化
/////////////////////////////////////
//alsa wav音频处理
// 默认wav音频头部数据
wave_pcm_hdr play_wav_hdr =
        {
                { 'R', 'I', 'F', 'F' },
                0,
                { 'W', 'A', 'V', 'E' },
                { 'f', 'm', 't', ' ' },
                16,
                1,
                1,
                16000,
                32000,
                2,
                16,
                { 'd', 'a', 't', 'a' },
                0
        };
wave_pcm_hdr capture_wav_hdr =
        {
                { 'R', 'I', 'F', 'F' },
                0,
                { 'W', 'A', 'V', 'E' },
                { 'f', 'm', 't', ' ' },
                16,
                1,
                1,
                16000,
                32000,
                2,
                16,
                { 'd', 'a', 't', 'a' },
                0
        };

extern struct SoundCtrl G_sndC = {
        // 音频设备描述
        NULL,
        NULL,

        NULL,
        NULL,
        NULL,
        // 线程
        NULL,
        NULL,
        // 缓存
        {NULL, NULL},
        0,
        0,      //pcmBufSize
        0,      //pcmBufFrames
        320,        //recPeriodsCount 最大period数，大概19秒
        0,
        0,          //recPerPeriodFrames
        0,
        // 状态
        {BufState_Empty, BufState_Empty}, //onPcmBufState 初始空
        0,
        0,
        0,
        0
};





// 处理并复制双声道资料到单声道pcm缓存
// 注意：frames是帧数不是字节数
void CopyStereoToMonoPcmBuf(char* des, char* src, int frames)
{
    int i = 0;
    short* rxP = src;//转换为short数组
    short* pcmP = des;
    for (i = 0; i < frames; i++)
    {
        pcmP[i] = rxP[i * 2];
        pcmP[i] *= 4;//放大声音
    }

//    // 测错误资料
//    int p = 0;
//    short* data = pcmP;
//    while(p < frames)
//    {
//        if(data[p] == 16009)
//        {
//            _printf("错误振幅:%d....................%d\n", p, data[p]);
//        }
//        p++;
//    }
}


/////////////////////////////////////
// 导出接口
/////////////////////////////////////


// 音频设备初始化
extern void RecordDeviceInit()
{
    int rc;
    int ret;
    int dir = 0;
    int channels = capture_wav_hdr.channels;
    int frequency = capture_wav_hdr.samples_per_sec;
    int bit = capture_wav_hdr.bits_per_sample;
    int frameSize = capture_wav_hdr.block_align;

    rc = snd_pcm_open(&G_sndC.recordHwHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0)
    {
        _error("PCM 设备打开失败！（snd_pcm_open）\n");
        exit(1);
    }

    snd_pcm_hw_params_alloca(&G_sndC.recordHwParams); //分配params结构体
    if (rc < 0)
    {
        _error("分配结构体失败！（snd_pcm_hw_params_alloca）\n");
        exit(1);
    }
    rc = snd_pcm_hw_params_any(G_sndC.recordHwHandle, G_sndC.recordHwParams);//初始化params
    if (rc < 0)
    {
        _error("初始化参数失败！（snd_pcm_hw_params_any）\n");
        exit(1);
    }
    rc = snd_pcm_hw_params_set_access(G_sndC.recordHwHandle, G_sndC.recordHwParams, SND_PCM_ACCESS_RW_INTERLEAVED); //初始化访问权限
    if (rc < 0)
    {
        _error("初始化访问权限失败！（sed_pcm_hw_set_access）\n");
        exit(1);
    }

    //采样位数
    _printf("设置采样位数：%d\n", bit);
    switch (bit / 8)
    {
        case 1:snd_pcm_hw_params_set_format(G_sndC.recordHwHandle, G_sndC.recordHwParams, SND_PCM_FORMAT_U8);
            break;
        case 2:snd_pcm_hw_params_set_format(G_sndC.recordHwHandle, G_sndC.recordHwParams, SND_PCM_FORMAT_S16_LE);
            break;
        case 3:snd_pcm_hw_params_set_format(G_sndC.recordHwHandle, G_sndC.recordHwParams, SND_PCM_FORMAT_S24_LE);
            break;
    }
    _printf("设置声道：%d\n", 2);
    rc = snd_pcm_hw_params_set_channels(G_sndC.recordHwHandle, G_sndC.recordHwParams, 2); //设置声道,1表示单声>道，2表示立体声
    if (rc < 0)
    {
        _error("设置声道数失败！（snd_pcm_hw_params_set_channels）\n");
        exit(1);
    }
    //frequency /= 2;
    _printf("设置采样率:%d\n", frequency);
    rc = snd_pcm_hw_params_set_rate_near(G_sndC.recordHwHandle, G_sndC.recordHwParams, &frequency, &dir); //设置>频率
    if (rc < 0)
    {
        _error("设置采样率失败！（snd_pcm_hw_params_set_rate_near）\n");
        exit(1);
    }
    rc = snd_pcm_hw_params(G_sndC.recordHwHandle, G_sndC.recordHwParams);
    if (rc < 0)
    {
        _error("获取采样率失败！（snd_pcm_hw_params）\n");
        exit(1);
    }
    rc = snd_pcm_hw_params_get_period_size(G_sndC.recordHwParams, &G_sndC.recPerPeriodFrames, &dir); //获取周期长度， 1 frame = channels * sample_size
    if (rc < 0)
    {
        _error("\nsnd_pcm_hw_params_get_period_size:");
        exit(1);
    }
    _printf("周期长度：%d, dir：%d\n", G_sndC.recPerPeriodFrames, dir);
    int wait;
    _printf("等待录音设备启动。。。\n");
    usleep(2000 * 1000);//设备开启后，等待2秒，bias电平
    _printf("启动完毕\n");
    // 计算语音采样缓存
    G_sndC.totalRecFramesCount = G_sndC.recPerPeriodFrames * G_sndC.totalRecPeriodsCount; //录音总frame数
    G_sndC.perPeriodBytes = G_sndC.recPerPeriodFrames * frameSize;                     //每period字节数
    G_sndC.pcmBufSize = PCM_BUF_SIZE;            //录音缓存大小
    G_sndC.pcmBufFrames = G_sndC.pcmBufSize / frameSize;
    G_sndC.pcmBuf[0] = malloc(PCM_BUF_SIZE);                    //分配语音采样数据缓存
    G_sndC.pcmBuf[1] = malloc(PCM_BUF_SIZE);
    _printf("单次录音总帧数：%d\n", G_sndC.totalRecFramesCount);
    _printf("分配PCM缓存大小：%d\n", G_sndC.pcmBufSize);
}


//功能：用alsa录音到文件
// 注意：pcmBufSize必须是单个frame字节数的整数倍
extern void Record()
{
    _printf("开始录音！\n");
    int times = 0;//记录循环次数
    int len = G_sndC.totalRecFramesCount; //录音采样点总数
    int err = 0;
    //char* rxBuf = malloc(G_sndC.pcmBufSize * 2);//接收语音资料，比最终缓存多一倍，用来存储双声道
    char* rxBuf = malloc(G_sndC.perPeriodBytes * 2);
    int rxBufEnd = 0;
    int switcher = 0;//缓存切换用

    //文件操作
    int totalFileDataBytes = 0;     //wav文件pcm总字节数
    int bytesWriteToFile = 0;   //本次写入字节数


    //持续录音
    while (1)
    {
        _printf("第：%d次\n", times);
        len = G_sndC.totalRecFramesCount; //录音采样点总数
        err = 0;
        G_sndC.pcmEnd = 0;
        rxBufEnd = 0;
        //打开文件
        FILE* fp = fopen("test.wav", "w+");
        fwrite(&capture_wav_hdr, 1, sizeof(capture_wav_hdr), fp); //如果要存成wav档，则写入文件头
        //memset(G_sndC.pcmBuf, 0, G_sndC.pcmBufSize);
        //memset(rxBuf, 0, G_sndC.pcmBufSize);
//        pthread_create(&G_recognizeThread, NULL, speechRecognize, NULL);
        sysUsecTime();
        // 循环读取录音数据，最多len个periods，超过会重新开始录音。
        while (len > 0)
        {
            int leftFrames = G_sndC.recPerPeriodFrames;
            rxBufEnd = 0;
            do
            {
                err = snd_pcm_readi(G_sndC.recordHwHandle, rxBuf + rxBufEnd, leftFrames);  //一次读取一个period
                if(err > 0)
                {
                    rxBufEnd += err * capture_wav_hdr.block_align * 2;//取得原始资料是双声道的
                    leftFrames -= err;
                }
//                if(leftFrames != 0)
//                    printf("leftFrames:%d\n", leftFrames);
            }while(leftFrames > 0);//读完整个period跳出
            len -= G_sndC.recPerPeriodFrames;

            //是否会超当前pcm缓存
            if(G_sndC.pcmBufSize < G_sndC.pcmEnd + G_sndC.perPeriodBytes)
            {
                _printf("切换缓存。\n");
                int needFrames = (G_sndC.pcmBufSize - G_sndC.pcmEnd) / capture_wav_hdr.block_align;//还可写入的frame数
                //取得资料转为单声道
                CopyStereoToMonoPcmBuf(G_sndC.pcmBuf[switcher] + G_sndC.pcmEnd, rxBuf, needFrames);
                rxBufEnd = needFrames * capture_wav_hdr.block_align * 2;//指向剩余资料。注意：直接读取的资料是双声道
                //存入文件
                bytesWriteToFile = needFrames * capture_wav_hdr.block_align;
                fwrite(G_sndC.pcmBuf[switcher] + G_sndC.pcmEnd, 1, bytesWriteToFile, fp);
                totalFileDataBytes += bytesWriteToFile;
                G_sndC.onPcmBufState[switcher] = BufState_Full;
                //切换缓存
                G_sndC.pcmEnd = 0;
                switcher = !switcher;
                //buf被在使用中。或者资料未被取用。等待解锁
                while(G_sndC.onPcmBufState[switcher] != BufState_Empty){
                    _printf("等待缓存空闲。。。%d\n", switcher);
                    usleep(200 * 1000);//等待200毫秒
                }
                CopyStereoToMonoPcmBuf(G_sndC.pcmBuf[switcher], rxBuf + rxBufEnd, G_sndC.recPerPeriodFrames - needFrames);//复制剩余
                G_sndC.pcmEnd += (G_sndC.recPerPeriodFrames - needFrames) * capture_wav_hdr.block_align;
                //存入文件
                bytesWriteToFile = (G_sndC.recPerPeriodFrames - needFrames) * capture_wav_hdr.block_align;
                fwrite(G_sndC.pcmBuf[switcher], 1, bytesWriteToFile, fp);
                totalFileDataBytes += bytesWriteToFile;
            }
            else {
                //取得资料转为单声道
                CopyStereoToMonoPcmBuf(G_sndC.pcmBuf[switcher] + G_sndC.pcmEnd, rxBuf, G_sndC.recPerPeriodFrames);
                //存入文件
                fwrite(G_sndC.pcmBuf[switcher] + G_sndC.pcmEnd, 1, G_sndC.perPeriodBytes, fp);
                totalFileDataBytes += G_sndC.perPeriodBytes;
                //更改缓存指针
                G_sndC.pcmEnd += G_sndC.perPeriodBytes;
                _printf("      写入文件单声道的字节数：%d\n", G_sndC.perPeriodBytes);
            }
            // 录音时间用完没？
            if(len <= 0) {
                _printf("本次录音时间用完，退出录音模式。\n");
                sysUsecTime();
                G_sndC.onRecord = 0;
            }
            //按了暂停键，进入暂停循环
            if (!G_sndC.onRecord)
            {
                _printf("非录音状态：%d\n", G_sndC.onRecord);

                //暂停录音，防止长期不读取采样，出现underrun错误
                if ((err = snd_pcm_pause(G_sndC.recordHwHandle, 1)) < 0)
                    _error("%s", snd_strerror(err));
                //结束文件写入
                fseek(fp, 40, SEEK_SET);//写入文件资料总长度
                fwrite(&totalFileDataBytes, 1, 4, fp);
                fseek(fp, 4, SEEK_SET);
                int fileSize = totalFileDataBytes + 36;
                fwrite(&fileSize, 1, 4, fp);
                fclose(fp);
                _printf("文件写入完毕。\n");
                while (1)
                {
                    sleepForFinishRecognize:
                    usleep(200 * 1000);//每秒 检测一下状态
                    if (G_sndC.onRecord)
                    {
                        _printf("需要切换到录音状态\n");
                        if (G_sndC.onRecognize)
                        {
                            _printf("还在识别再等一会\n");
                            continue;   //如果还在识别，再等一个循环
                        }
                        else
                        {
                            _printf("开始下次录音\n");
                            //恢复录音
                            if ((err = snd_pcm_pause(G_sndC.recordHwHandle, 0)) < 0)
                                _printf("%s", snd_strerror(err));
                            goto nextRec;  //重新开始录音
                        }
                    }
                    else
                        _printf(">>>>>>>录音暂停中。。。\n");
                }
            }
            if (G_sndC.onKillAllThread)
            {
                _printf("退出录音线程:%d\n", G_sndC.recordThread);

                free(rxBuf);
                free(G_sndC.pcmBuf[0]);
                free(G_sndC.pcmBuf[1]);
                goto exitRecord;
            }
        }

        //判断是否正在识别中，等待识别完成
        if (G_sndC.onRecognize)
            goto sleepForFinishRecognize;
        nextRec:
        times++;
    }



    exitRecord:
    _printf("录音结束\n");
    pthread_exit(G_sndC.recordThread);
}



//int main(int argc, char *argv[])
//{
//    printf("说明：必须有2个参数。\n");
//    printf("    1:录音时间。\n");
//    printf("    2:保存的wav文件名。\n");
//    printf("    例子：./Recorder 5 test.wav\n");
//    if(argv[1] == NULL || argv[2] == NULL)
//    {
//        _error("参数不够！！！\n");
//        exit(1);
//    }
//
//    G_sndC.onRecord = 1;
//    RecordDeviceInit();
//    int thrRes;//创建的线程返回值
//    thrRes = pthread_create(&G_sndC.recordThread, NULL, Record, argv[2]);
//    if (thrRes != 0)
//    {
//        _error("Thread creation failed!");
//        exit(EXIT_FAILURE);
//    }
//    sleep(atoi(argv[1]));
//    G_sndC.onRecord = 0;
//    G_sndC.onKillAllThread = 1;
//    sleep(2);
//}