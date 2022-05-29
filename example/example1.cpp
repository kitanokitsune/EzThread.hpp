/*
example1.cpp: Easy Thread C++ Class Library Sample Program
Note:
  This library is intended for old C++ compilers.
  If your compiler is C++11 compliant, you can use std::thread instead.

This sample demnstrates how to use the library:

  (1) Include "EzThread.hpp"
  (2) Declare a derived class that inherits from EzThreadBase class.
  (3) For safty, add a code to call join() in a destructor of the derived class.
  (4) Implement app() method in the derived class, which is executed on a thread.
  (5) Instantiate the derived class, and start a thread with run() method.


How to compile:

 GNU:           g++ example1.cpp -lpthread
 MinGW(pthread):g++ -static -static-libstdc++ -static-libgcc example1.cpp -lpthread
 MinGW:         g++ -static -static-libstdc++ -static-libgcc example1.cpp -DUSE_WIN_THREAD
 Microsoft:     cl /MT example1.cpp
 Borland:       bcc32 -WM example1.cpp
 Embarcadero:   bcc32c -tCM example1.cpp
 Digital Mars:  dmc example1.cpp -D_MT=1
 Open Watcom:   wcl386 -bm example1.cpp

*/

/* ------------------------------------------------------------------------- */

#include <stdio.h>         /* printf() */
#include "millisleep.h"    /* millisleep() */

#ifdef __DMC__
#  include "dmc_safe_printf.h" /* patch for Digial Mars Compiler's printf() */
#endif

/**********************************************************************
  * (1) Include "EzThread.hpp"                                         *
  **********************************************************************/

#include "../EzThread.hpp"

 /**********************************************************************
  * (2) Declare a derived class that inherits from EzThreadBase class. *
  **********************************************************************/

class MyClass : public EzThreadBase
{
  public:
    // Arguments and return values are passed via member variables (their names are arbitrary).
    // Variables passing values from/to the thread may be better declared as 'volatile'.
    volatile int    m_Input1;  /* (e.g. thread ID)    */
    volatile int    m_Input2;  /* (e.g. loop count)   */
    volatile int    m_Output;  /* (e.g. return value) */

    MyClass(int id, int num) { m_Input1 = id; m_Input2 = num; };

 /**********************************************************************************
  * (3) For safty, it is recommended to add a code to call join() in a destructor. *
  **********************************************************************************/

/* join() prevents unintentional destruction of running threads when instances are deleted */
    ~MyClass() { join(); };

    void app();  /* MyClass::app() is defined below */
};

 /***********************************************************************************
  * (4) Implement app() method in the derived class, which is executed on a thread. *
  ***********************************************************************************/

void MyClass::app()
{
/* --- Write a code you want to execute on a thread. --- */
    for(int i = 1 ; i <= m_Input2 ; i++) {
        millisleep(2500);
        printf("[Th%d] loop %d\n", m_Input1, i);
    }

/*-- Use a member variable to pass a return value from a thread to the invoker. --*/
    m_Output = 12345;
    printf("[Th%d] exit\n", m_Input1);
};

/* ------------------------------------------------------------------------- */

int main()
{

 /******************************************************************************
  * (5) Instantiate the derived class, and start a thread with run() method.   *
  ******************************************************************************/

    MyClass Th1(1, 6), Th2(2, 8);              // instantiate (auto allocation)
    EzThreadBase *pTh3 = new MyClass(3, 4);    // instantiate (dynamic allocation)

                         Th1.run();     // start the thread #1
    millisleep(500);     Th2.run();     // start the thread #2 after 500msec
    millisleep(500);     pTh3->run();   // start the thread #3 after another 500msec

    // Note that there is a time lag between run() call and thread creation.
    // If you like to know that a thread has been started or not, check the member
    // function status() is 2 or higher. Please note that status()==1 does not mean
    // the thread is created.
    // status() = 0:unexecuted, 1:creating, 2:created and running, 4:finished, 8:joined

    printf("<< Status: Th1=%d, Th2=%d, Th3=%d >>\n",
           Th1.status(), Th2.status(), pTh3->status());

    delete pTh3; // What happens if the instance #3 with an active thread is deleted?
    printf("<< Th3 has been deleted by main() >>\n");

    // Note that deleting an instance will destroy its app() entity.
    // In general, the program may crash if an instance with an active thread is deleted.
    // To prevent the program from crashing in the above situation:
    //   call join() method before deleting the instance,
    //   or put join() method in a destructor of the derived class.
    // In this demo, join() method is put in the destructor,
    // so 'delete pTh3' waits for the thread #3 to finished before deleting the instance.


    // In this demo, a return value is passed via a member variable 'm_Output'.
    // The content of 'm_Output' before the thread finishes is undefined.

    Th1.join();  // join the thread #1 to determin the content of 'Th1.m_Output'.

    printf("<< Th1 %s m_Output = %d >>\n",
           Th1.status() & 0xC ? "is done.     The output is valid:  "
                              : "is not done. The output is invalid:",
           Th1.m_Output);
    printf("<< Th2 %s m_Output = %d >>\n",
           Th2.status() & 0xC ? "is done.     The output is valid:  "
                              : "is not done. The output is invalid:",
           Th2.m_Output);

    // Note that Th2 is not joined yet (and may still be running),
    // so the content of 'Th2.m_Output' is not guaranteed.


    printf("<< Status: Th1=%d, Th2=%d >>\n",  Th1.status(), Th2.status());
    return 0;

    // Note that the instances #1 and #2 are implicitly deleted here.
}
