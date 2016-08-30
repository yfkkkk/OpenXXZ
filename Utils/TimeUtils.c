//
// Created by yf on 16-7-14.
//
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
//精确显示系统时间
extern void sysUsecTime()
{
    struct timeval    tv;
    struct timezone tz;
    struct tm         *p;

    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);
    printf("%d年%d月%d日%d点%d分%d秒.%ld毫秒\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec/1000);
}