#ifndef __DMC_SAFE_PRINTF_H__
#define __DMC_SAFE_PRINTF_H__
/**************************************************************************
  In general printf() is thread-safe in most standard C/C++ compilers.
  It is defined in a C11 standard or POSIX.
  However Digital Mars Compiler's printf() is not thread-safe.
  This header file provides thread-safe printf() to DMC.
 **************************************************************************/
#include <stdio.h>
#include <windows.h>

#define printf       dmc_safe_printf
#define rsc_token_t  LONG

static inline int dmc_acquire(volatile rsc_token_t &rsc) {
    while (InterlockedExchange(&rsc, 1)) Sleep(0);
    return -1;
}

static inline void dmc_release(volatile rsc_token_t &rsc) {
    InterlockedExchange(&rsc, 0);
}

volatile rsc_token_t token_printf = 0;

int dmc_safe_printf(const char * fmt, ...) {
    int n;
    va_list args;
    va_start(args, fmt);
    dmc_acquire(token_printf);
    n = vprintf(fmt, args);
    dmc_release(token_printf);
    va_end(args);
    return n;
}

#endif /* __DMC_SAFE_PRINTF_H__ */