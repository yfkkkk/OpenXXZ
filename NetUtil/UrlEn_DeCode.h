//
// Created by yf on 16-7-2.
//

#ifndef TEST_URLEN_DECODE_H
#define TEST_URLEN_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

int php_url_decode(char *str, int len);
char *php_url_encode(char const *s, int len, int *new_length);

#ifdef __cplusplus
}
#endif

#endif //TEST_URLEN_DECODE_H
