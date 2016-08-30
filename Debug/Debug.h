//
// Created by yf on 16-7-14.
//

#ifndef TEST_DEBUG_H
#define TEST_DEBUG_H
# include <stdio.h>
#define _error printf
#ifndef _error
static int _error(char *fmt, ...)
{
    return 0;
}
#endif // !_error

//#define _printf printf
#ifndef _printf
static int _printf(char *fmt, ...)
{
    return 0;
}
#endif // !_printf
#endif //TEST_DEBUG_H
