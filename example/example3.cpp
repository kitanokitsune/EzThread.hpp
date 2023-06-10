#include <stdio.h>
#include <time.h>
#include "../EzThread.hpp"

#ifdef __DMC__
#  include "dmc_safe_printf.h" /* patch for Digial Mars Compiler's printf() */
#endif

#define VBOOL_t  volatile bool

/*----------------------------*/
/*     thread function #1     */
/*----------------------------*/
void threadfunc1(VBOOL_t *pReady) /* the memory pointed by pReady is shared with main() */
{
    printf("<< Start Thread1 >>\n");

    time_t t = clock();
    while(*pReady == true) {
    /* do something... */
        if (clock() > t+(static_cast<time_t>(CLOCKS_PER_SEC)>>2)) {
            printf("|"); fflush(stdout);
            t=clock();
        }
    }

    printf("<< End Thread1 >>\n");
}

/*----------------------------*/
/*     thread function #2     */
/*----------------------------*/
void threadfunc2(VBOOL_t *pReady)
{
    printf("<< Start Thread2 >>\n");

    time_t t = clock();
    while(*pReady == true) {
    /* do something... */
        if (clock() > t+static_cast<time_t>(CLOCKS_PER_SEC)) {
            printf("-"); fflush(stdout);
            t=clock();
        }
    }

    printf("<< End Thread2 >>\n");
}

/*----------------------------*/
/*            main            */
/*----------------------------*/
int main(void)
{
    VBOOL_t ready = true;
    EzThread<VBOOL_t *> obj1(&threadfunc1, &ready);  /* begin thread */
    EzThread<VBOOL_t *> obj2(&threadfunc2, &ready);  /* begin thread */

    printf("==== main() : Thread has been created ====\n");

    getchar();  /* Enter key to stop */

    printf("==== main() : Give an instruction to quit ====\n");
    ready = false;

    return 0;
}
