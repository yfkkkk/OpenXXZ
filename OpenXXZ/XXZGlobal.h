//
// Created by yf on 16-8-25.
//

#ifndef TEST_XXZGLOBAL_H
#define TEST_XXZGLOBAL_H

typedef struct XXZReqParams
{
    char* key;
    char* devid;
    char* user;
    char* nickname;
    char* city;
    char* text;
}XXZReqParams;

enum ResDataType
{
    text = 1,
    weather = 2,
    news = 3,
    shop = 4,
    path = 5,
    phone = 6,
    music = 7,
    webShop = 8
};
#endif //TEST_XXZGLOBAL_H
