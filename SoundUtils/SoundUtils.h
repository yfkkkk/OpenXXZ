//
// Created by yf on 16-7-13.
//

#ifndef TEST_SOUNDUTILS_H
#define TEST_SOUNDUTILS_H
#include <asoundlib.h> //wav   alsa
/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
    char    riff[4];                // = "RIFF"
    int		size_8;                 // = FileSize - 8
    char    wave[4];                // = "WAVE"
    char    fmt[4];                 // = "fmt "
    int		fmt_size;		// = 下一个结构体的大小 : 16

    short int   format_tag;             // = PCM : 1
    short int   channels;               // = 通道数 : 1
    int		samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
    int		avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
    short int   block_align;            // = 每采样点字节数 : wBitsPerSample / 8
    short int   bits_per_sample;        // = 量化比特数: 8 | 16

    char    data[4];                // = "data";
    int		data_size;              // = 纯数据长度 : FileSize - 44
} wave_pcm_hdr;

// 缓存状态
enum BufState
{
    BufState_Empty,
    BufState_Locked,
    BufState_Full
};

struct SoundCtrl
{
    // 音频设备描述
    snd_pcm_t* recordHwHandle;              //PCI设备句柄
    snd_pcm_hw_params_t* recordHwParams;    //硬件信息和PCM流配置


    snd_pcm_t* playHwHandle;                //PCI设备句柄
    snd_pcm_hw_params_t* playHwParams;      //硬件信息和PCM流配置
    snd_pcm_uframes_t playHwFrames;

    // 线程
    pthread_t recordThread;                 //录音线程
    pthread_t recognizeThread;              //识别播放线程

    // 缓存
    char* pcmBuf[2];                           //pcm缓存
    int pcmEnd;                             //pcm数据末尾指针

    int pcmBufSize;//pcm缓存字节数
    int pcmBufFrames;//pcm缓存frame总数
    int totalRecPeriodsCount;                    //总period数，限制录音总长度
    int totalRecFramesCount;
    snd_pcm_uframes_t recPerPeriodFrames;      //每period字节数
    int perPeriodBytes;                     //单period字节数

    // 状态
    enum BufState onPcmBufState[2];
    char onRecord;
    char onRecognize;  //识别过程中
    char onNetConfig;//一键配网，按钮标志
    char onKillAllThread;    //杀所有线程标志
};

extern struct SoundCtrl G_sndC;

#endif //TEST_SOUNDUTILS_H
