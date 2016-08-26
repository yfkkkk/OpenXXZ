//
// Created by yf on 16-6-29.
//


#include "MAC.h"

int GetMACAddr(char* MAC, char* device)
{
    struct   ifreq   ifreq;
    int   sock;

    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0)
    {
        perror( "socket ");
        return   -2;
    }
    strcpy(ifreq.ifr_name, device);
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) <0)
    {
        perror( "ioctl ");
        return   -3;
    }
    sprintf(MAC, "%02x%02x%02x%02x%02x%02x\0",
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[0],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[1],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[2],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[3],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[4],
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[5]);
    printf("%s\n", MAC);
    return 0;
}