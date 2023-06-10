/***********************************************************************
Create threads as many as possible : CAUTION! This may freeze your computer!

How to compile:

 GNU:           g++ example4.cpp -pthread
 MinGW(pthread):g++ -static -static-libstdc++ -static-libgcc example4.cpp -pthread
 MinGW:         g++ -static -static-libstdc++ -static-libgcc example4.cpp -DUSE_WIN_THREAD
 Microsoft:     cl /MT /EHsc example4.cpp
 Borland:       bcc32 -WM example4.cpp
 Embarcadero:   bcc32c -tCM example4.cpp
 Digital Mars:  dmc example4.cpp -D_MT=1
 Open Watcom:   wcl386 -bm -xs example4.cpp
***********************************************************************/

#define MAX_TRY_THREADS  1560   /* !!! For bcc32 setting over 1560 cause freeze !!! */

#include <stdio.h>      /* printf() */
#include <vector>       /* std::vector */
#include "../EzThread.hpp"

#ifdef __DMC__
#  include "dmc_safe_printf.h" /* patch for Digial Mars Compiler's printf() */
#endif

#ifdef __WATCOMC__
#  define new(x)    new      /* open watcom does not support std::nothrow */
#endif

/* ------------------------------------------------------------------------- */

static volatile int go = 0;

void thread_func(int n)
{
    while(!go) EzMutex::millisleep(10);  /* Wait for a go signal */
    for (int i=1 ; i<=10; i++) {
        EzMutex::millisleep(500);
        printf("<< t%05d >> loop %d\n", n, i);
    }
    printf("<< t%05d >> exit\n", n);
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    std::vector<EzThread<int> *> objs;
    EzThread<int> *p;
    int n;

    n = 0;
    while (n < MAX_TRY_THREADS) {
        if ((p = new (std::nothrow) EzThread<int>) == NULL) {
            /* allocation failure */
            fprintf(stderr, "Reached new limitation\n");
            break;
        }
        if (p->run(&thread_func, n)) {
            /* thread creation failure */
            fprintf(stderr, "Reached _beginthreadex limitation\n");
            delete p;
            break;
        }
        objs.push_back(p);  /* Store a thread instance pointer */
        printf("Thread %05d has been created\n", n); fflush(stdout);
        n++;
    };

    fprintf(stderr,"***** n=%d ******\n", n);
    EzMutex::millisleep(5000);
    go = 1;  /* Signal for all the threads to start to count up */

    /* Delete all the instances */
    while (!objs.empty()) {
        p = static_cast<EzThread<int> *>(objs.back());
        delete p;
        objs.pop_back();
    }
    fprintf(stderr,"complete!\n");

    return 0;
}

