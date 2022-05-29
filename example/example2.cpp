/*****************************************************************************
      example2.cpp : EzThread Example: An Easy-to-Use Thread Class
 ----------------------------------------------------------------------------
    EzThread is a wrapper of EzThreadBase to use threads easily.
    Only you should do is:
    (1) Define a thread function of type 'void funcname(TYPE)'.
    (2) Instantiate an EzThread<TYPE> object with the function as an argument.
 *****************************************************************************/

#include <stdio.h>      /* printf() */
#include "millisleep.h" /* millisleep() */
#include "../EzThread.hpp"

#ifdef __DMC__
#  include "dmc_safe_printf.h" /* patch for Digial Mars Compiler's printf() */
#endif

/* -------------------------- thread function ------------------------------- */
/* A thread function must be a type of 'void funcname(TYPE)' where TYPE is a  */
/*  type of the argument.                                                     */
/* -------------------------------------------------------------------------- */
void thread_func(int n)  /* n is a local variable (call by value) */
{
    for (int i=1 ; i<=3; i++) {
        millisleep(1000);
        printf("<< t%d >> loop %d\n", n, i);
    }
    printf("<< t%d >> exit\n", n);
}

/* ---------------------------------- main ---------------------------------- */
int main()
{
    /* Start a thread at the same time as instantiation */
    EzThread<int> t1(&thread_func, 1); /* EzThread<TYPE>: where TYPE must be the same as the argument type of thread_func. */

    /* Start a thread with an interval after instantiation */
    EzThread<int> *t2 = new EzThread<int>;   /* instantiate an empty EzThread instance */
    millisleep(1000);              /* 1 sec interval */
    t2->run(&thread_func, 2);     /* start a thread */

    /* (optional) Wait until a thread finishes. */
    // t1.wait();
    // t2->wait();

    /* Calling wait() is optinal because wait() is automatically called at deletion.*/
    /* wait() function can synchronize some action to the end of the thread. */

    printf("done.\n");

    delete t2;  /* Note: t2->wait() is automatically called at this moment. */

    return 0;
}
