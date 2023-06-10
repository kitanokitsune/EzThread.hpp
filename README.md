# EzThread.hpp

An *Easy-to-Use* and *Header-Only* Thread C++ Class Library


# DEMO
Sample Code:
```c++
#include <stdio.h>
#include <time.h>
#include "EzThread.hpp"  /* This library */

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
        if (clock() > t+(CLOCKS_PER_SEC>>2)) {
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
        if (clock() > t+CLOCKS_PER_SEC) {
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
    EzThread<VBOOL_t *> obj1(&threadfunc1, &ready);  /* begin thread #1 */
    EzThread<VBOOL_t *> obj2(&threadfunc2, &ready);  /* begin thread #2 */

    getchar();  /* Enter key to stop */
    ready = false;

    return 0;
}
```
Output:
```text
<< Start Thread1 >>
<< Start Thread2 >>
|||-||||-||||-||||-||||-||||-||||-|
<< End Thread1 >>
<< End Thread2 >>
```

# Features

* Thread management is encapsulated and hidden.
* Header only library
* Platform independent (It would be ...)
* Legacy C++ compilers such as *Borland*, *Digital Mars* and *Open Watcom* are supported.
* **[ Class** ***EzThreadBase*** **]** Abstract class to produce a runnable object on a thread. (see [example1.cpp](./example/example1.cpp))
* **[ Class** ***EzThread*** **]** Class template for any function to be runnable on a thread in a simple manner. (see [example2.cpp](./example/example2.cpp))

# Requirement

* C++ compiler (g++, Visual C++, Borland C++, Digital Mars C++ and Open Watcom C++ are tested)

# Installation

Copy the **EzThread.hpp** file in the same directory as the source files.

# Usage

Include the **EzThread.hpp** in your C++ source file and write a code. Please see [sample files](./example) for details.  

### Compile
On compilation you may need some option switch. Examples are shown below.

| Compiler          | Commandline              |
| :----             | :----                    |
|  GNU g++          | g++ *main.cpp* -pthread |
|  Microsoft VC++   | cl /MT *main.cpp*        |
|  Borland C++ 5.5  | bcc32 -WM *main.cpp*     |
|  Embarcadero      | bcc32c -tCM *main.cpp*   |
|  Digital Mars C++ | dmc *main.cpp* -D_MT=1   |
|  Open Watcom      | wcl386 -bm *main.cpp*    |


# Class Description
There are three classes defined in this library.  
+ **EzThread&lt;**_TYPE_**&gt;**
+ [**EzThreadBase**](#ezthreadbase)
+ [**EzMutex**](#ezmutex)

## EzThread&lt;TYPE&gt;
*EzThread&lt;TYPE&gt;* enables any function to run on a thread.  
*TYPE* must be the same as the argument type of the thread function.  
--> See [example2.cpp](./example/example2.cpp) , [example3.cpp](./example/example3.cpp) , [example4.cpp](./example/example4.cpp)

| Member | Description |
| :---   | :---        |
| **EzThread**() | A constructor. It generates an empty object. User can run a thread function by **run**() method. |
| <nobr> **EzThread**(*func*, *arg*) </nobr> | A constructor. It generates an object and starts the function ***func***(***arg***) on a thread immediately. |
| int **run**(*func*, *arg*) | Start the function ***func***(***arg***) on a thread |
| int **rerun**() | Rerun the function which status() is EZTH_JOINED (8) |
| int **wait**() | Wait until the thread finishes. **wait**() is automatically called at the object deletion. Also user can call **wait**() anywhere to join the thread. <br> ret=0:success,  -1:error |
| int **status**() | Get thread status <br> ret=0:unexecuted, 1:creating, 2:running, 4:finished, 8:joined |

## EzThreadBase
*EzThreadBase* is an abstract class which has thread management functions. You can flexibly implement your own thread class derived from it without thread management.  
--> See [example1.cpp](./example/example1.cpp)

| Member | Description |
| :---   | :---        |
| void **app**()  | User function that runs on a thread. The **EzThreadBase::app**() is a pure virtual function so that user should override it in the derived class. |
| int **run**() | Create and start a thread. Overriding **run**() method is prohibitted. <br> ret=0:success,  -1:error |
| int **join**() | Wait until the thread finishes. The **join**() is not called automatically at object deletion so that user should confirm the thread is done.  Overriding **join**() method is prohibitted.<br> ret=0:success,  -1:error |
| int **status**() | Get thread status <br> ret=0:unexecuted, 1:creating, 2:running, 4:finished, 8:joined |
| HANDLE **get_win_thread_handle**() | (**Windows only**) A handle returned by _beginthredex() |
| pthread_t **get_posix_thread_handle**() | (**POSIX only**) A handle returned by pthread_create() |

## EzMutex
*EzMutex* is a companion class which provides a mutual exclusion mechanism.

| Member | Description |
| :---   | :---        |
| void **lock**()  | Acquire a *mutex* of this instance. This method blocks (pauses) until the *mutex* can be acquired. |
| bool **try_lock**() | Try to acquire a *mutex* of this instance. This method returns immediately regardless of whether the *mutex* can be acquired or not.<br>**true** is returned if the *mutex* was sucessfully acquired, otherwise **false** is returned. |
| void **unlock**() | Release a *mutex* of this instance. |

EzMutex also provides two sleep utilities.
| Member | Description |
| :---   | :---        |
| EzMutex::**Wait**() | Yield the execution priority to other threads and sleep for a minimal period. |
| EzMutex::**millisleep**(unsigned long *msec*) | Sleep for *msec* milliseconds.<br>**Note:** On Windows platforms, due to Windows timer limitations, the resolution of the sleep interval is typically about 16 ms. |



# Note

This library is ***experimental*** and ***not guaranteed***.  
If your compiler is **C++11** compliant, you should use **std::thread** instead.

# Author

* [Kitanokitsune](https://github.com/kitanokitsune)  

# License

**EzThread.hpp** is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).  
```text
Copyright (c) 2022, 2023 Kintanokitsune

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```