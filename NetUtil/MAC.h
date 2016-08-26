//
// Created by yf on 16-6-29.
//

#ifndef TEST_MAC_H
#define TEST_MAC_H
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
int GetMACAddr(char* MAC, char* device);
#endif //TEST_MAC_H
