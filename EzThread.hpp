#ifndef __EZTHREAD_HPP__
#define __EZTHREAD_HPP__
/*****************************************************************************
EzThread: Easy Thread C++ Class Library

Note:
  This library is intended for old C++ compilers.
  If your compiler is C++11 compliant, you should use std::thread instead.
------------------------------------------------------------------------------
How to use the library:

  (1) Include "EzThread.hpp"
  (2) Declare a derived class that inherits from EzThreadBase class.
  (3) For safty, add a code to call join() in a destructor of the derived class.
  (4) Implement app() method in the derived class, which is executed on a thread.
  (5) Instantiate the derived class, and start a thread with run() method.
------------------------------------------------------------------------------
Command line switches for each C++ compiler:

  GNU:          g++ main.cpp -lpthread
  Microsoft:    cl /MT main.cpp
  Borland:      bcc32 -WM main.cpp
  Embarcadero:  bcc32c -tCM main.cpp
  Digital Mars: dmc main.cpp -D_MT=1
  Open Watcom:  wcl386 -bm main.cpp

******************************************************************************
EzThread.hpp is under MIT license
----------------------------------
Copyright (c) 2022 Kintanokitsune

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
******************************************************************************/

/* -------------------------------------------------------------------------- */

/*****************************************************************************
      MISC DEFINITION
 *****************************************************************************/

/* -------------------------------------------------------------------------- */
/*  SELECT THREAD MODEL: Determin Windows Thread Model or POSIX Thread model, */
/*                        and also define memory barrier method.              */
/* -------------------------------------------------------------------------- */
#if defined(_WIN32) && !defined(__GNUC__)
#  ifndef USE_WIN_THREAD
#     define USE_WIN_THREAD
#  endif
#endif

#ifdef USE_WIN_THREAD
#   include <windows.h>
#   include <process.h>
#   define EZ_MEM_BARRIER() MemoryBarrier()
#   define threadhandle_t  HANDLE
//#   define threadid_t unsigned
#else
#   include <pthread.h>
#   define EZ_MEM_BARRIER() __sync_synchronize()
#   define threadhandle_t  pthread_t
//#   define threadid_t pthread_t
#endif

#ifdef USE_WIN_THREAD
#  if !defined(_MSC_VER) && !defined(__GNUC__)
    // static inline void MemoryBarrier(void) { LONG Barrier; asm { xchg Barrier, eax };}
    static inline void MemoryBarrier(void)
    {
        LONG Barrier;
        InterlockedExchange(&Barrier, 0L);
    }
#  endif
#endif

/* -------------------------------------------------------------------------- */
/*  Overriding run()/join() method is prohibited.                             */
/*  c++11 can avoid overriding them with virtual/final keyword.               */
/* -------------------------------------------------------------------------- */

#ifndef __cplusplus
#   define __DO_NOT_OVERRIDE__
#   define __OVERRIDE_IS_PROHIBITED__
#elif __cplusplus >= 201103L                /* c++11 is supported     */
#   define __DO_NOT_OVERRIDE__ virtual
#   define __OVERRIDE_IS_PROHIBITED__ final
#else                                       /* c++11 is not supported */
#   define __DO_NOT_OVERRIDE__
#   define __OVERRIDE_IS_PROHIBITED__
#endif

/* --------------------------- thread status -------------------------------- */

#define EZTH_UNEXEC             0x0
#define EZTH_CREATING           0x1
#define EZTH_RUNNING            0x2
#define EZTH_FINISHED           0x4
#define EZTH_JOINED             0x8

/*****************************************************************************
      CLASS DEFINITION : EzThreadBase
 *****************************************************************************/

class EzThreadBase
{
/* --------------- force EzThreadBase to be Noncopyable-class --------------- */
  private:

    EzThreadBase(const EzThreadBase& src);
    EzThreadBase& operator=(const EzThreadBase& src);

/* ----------------------- private member variables ------------------------- */

    // threadid_t     m_ThreadId_;      /* unique ID for each thread             */
    threadhandle_t m_ThreadHandle_;  /* Win:HANDLE POSIX:pthread_t            */
    volatile int m_ThreadState_;     /* 0:unexecuted, 1:creating, 2:running, 4:finished, 8:joined */
    volatile int m_IsThreadCreated_; /* 0:joined or not created, -1:created   */

/* -------------------------------------------------------------------------- */
/*   FUNCTION :  __ThreadFuncWrapper()                                        */
/*       __ThreadFuncWrapper() is a static member function                    */
/*       which invokes app() method in a thread.                              */
/* -------------------------------------------------------------------------- */

#ifdef USE_WIN_THREAD
    static unsigned __stdcall __ThreadFuncWrapper(void* arg) {
        static_cast<EzThreadBase *>(arg)->m_ThreadState_ = EZTH_RUNNING;
        EZ_MEM_BARRIER();
        static_cast<EzThreadBase *>(arg)->app();
        EZ_MEM_BARRIER();
        static_cast<EzThreadBase *>(arg)->m_ThreadState_ = EZTH_FINISHED;
        return 0;
    };
#else
    static void* __ThreadFuncWrapper(void *arg) {
        static_cast<EzThreadBase *>(arg)->m_ThreadState_ = EZTH_RUNNING;
        EZ_MEM_BARRIER();
        static_cast<EzThreadBase *>(arg)->app();
        EZ_MEM_BARRIER();
        static_cast<EzThreadBase *>(arg)->m_ThreadState_ = EZTH_FINISHED;
        return NULL;
    };
#endif

/* -------------------------------------------------------------------------- */
/*   FUNCTION: app                                                            */
/*   A user routine to be executed on a thread.                               */
/*   It should be a pure virtual function to force the user to implement it.  */
/* -------------------------------------------------------------------------- */

  protected:

    virtual void app() = 0;

/* -------------------------------------------------------------------------- */
/*   CONSTRUCTOR                                                              */
/* -------------------------------------------------------------------------- */

    EzThreadBase() {
        m_IsThreadCreated_ = m_ThreadState_ = 0;
#ifdef USE_WIN_THREAD
        m_ThreadHandle_ = NULL;
        // m_ThreadId_ = 0;
#else
        m_ThreadHandle_ = pthread_self();
        // m_ThreadId_ = m_ThreadHandle_;
#endif
    };

/* -------------------------------------------------------------------------- */
/*   DESTRUCTOR                                                               */
/*       This destructor must be a virtual function in order that             */
/*       a destructor in the derived class never fail to be called.           */
/* -------------------------------------------------------------------------- */

  public:

    virtual ~EzThreadBase() {
#ifdef USE_WIN_THREAD
        if (m_ThreadHandle_) {
            CloseHandle(m_ThreadHandle_);
            // m_ThreadHandle_ = NULL;
        }
#else
        if (!pthread_equal(m_ThreadHandle_, pthread_self())) {
            pthread_detach(m_ThreadHandle_);
            // m_ThreadHandle_ = pthread_self();
        }
#endif
    };

/* -------------------------------------------------------------------------- */
/*   FUNCTION: get_thread_id                                                  */
/* -------------------------------------------------------------------------- */

    // threadid_t get_thread_id() const { return m_ThreadId_; }

/* -------------------------------------------------------------------------- */
/*   FUNCTION: get_[win|posix]_thread_handle                                  */
/* -------------------------------------------------------------------------- */

#ifdef USE_WIN_THREAD
    HANDLE get_win_thread_handle() const { return m_ThreadHandle_; }
#else
    pthread_t get_posix_thread_handle() const { return m_ThreadHandle_; }
#endif

/* -------------------------------------------------------------------------- */
/*   FUNCTION: status                                                         */
/*      get the status of this thread                                         */
/*   return value:                                                            */
/*      0:unexecuted, 1:creating, 2:created and running, 4:finished, 8:joined */
/* -------------------------------------------------------------------------- */

    int status() const { return m_ThreadState_; }

/* -------------------------------------------------------------------------- */
/*   FUNCTION: run                                                            */
/*      This function creates and starts a thread.                            */
/*      > Invokes app() method on a thread via __ThreadFuncWrapper()          */
/*      Return value :  0:success  -1:error                                   */
/* -------------------------------------------------------------------------- */

    __DO_NOT_OVERRIDE__  int run()  __OVERRIDE_IS_PROHIBITED__
    {
        if(!m_IsThreadCreated_) {
            m_ThreadState_ = EZTH_CREATING;
            EZ_MEM_BARRIER();
#ifdef USE_WIN_THREAD
            unsigned dummy;  /* Digital Mars requires the 6th arg of _beginthreadex. */
                             /* Other compilers do not require it (allow NULL).      */
            m_ThreadHandle_ = (HANDLE)_beginthreadex(NULL, 0, &__ThreadFuncWrapper,
                                                  this, 0, &dummy);
            if (m_ThreadHandle_ == (HANDLE)0L)    /* Fail */
            {
                m_ThreadHandle_ = NULL;
                m_ThreadState_ = EZTH_UNEXEC;
            } else {                              /* Success */
                m_IsThreadCreated_ = -1;
            }
#else
            if( pthread_create(&m_ThreadHandle_, NULL,
                               &__ThreadFuncWrapper,
                               this) /* !=0 */ )  /* Fail */
            {
                m_ThreadHandle_ = pthread_self();
                m_ThreadState_ = EZTH_UNEXEC;
            } else {                              /* Success */
                m_IsThreadCreated_ = -1;
                // m_ThreadId_ = m_ThreadHandle_;
            }
#endif
        }
        return (m_IsThreadCreated_ ? 0 : -1);
    };

/* -------------------------------------------------------------------------- */
/*   FUNCTION: join                                                           */
/*      This function waits until the thread finishes.                        */
/*      Return value :  0:success  -1:error                                   */
/* -------------------------------------------------------------------------- */

    __DO_NOT_OVERRIDE__  int join()  __OVERRIDE_IS_PROHIBITED__
    {
        if(m_IsThreadCreated_) {
#ifdef USE_WIN_THREAD
            if (m_ThreadHandle_) {
                if (WaitForSingleObject(m_ThreadHandle_, INFINITE) == WAIT_OBJECT_0) {
                    CloseHandle(m_ThreadHandle_);
                    m_ThreadHandle_ = NULL;
                    m_IsThreadCreated_ = 0;
                    EZ_MEM_BARRIER();
                    m_ThreadState_ = EZTH_JOINED;
                }
            }
#else
            if (!pthread_equal(m_ThreadHandle_, pthread_self())) {
                if (pthread_join(m_ThreadHandle_, NULL) == 0) {
                    m_ThreadHandle_ = pthread_self();
                    m_IsThreadCreated_ = 0;
                    EZ_MEM_BARRIER();
                    m_ThreadState_ = EZTH_JOINED;
                }
            }
#endif
        }
        return (m_IsThreadCreated_ ? -1 : 0);
    };

};


/* -------------------------------------------------------------------------- */

/*****************************************************************************
      CLASS DEFINITION : EzThread
 *****************************************************************************/

template <typename TYPE>
class EzThread : private EzThreadBase
{
  private:
    void (*m_func)(TYPE);
    TYPE m_arg;
    void app() { m_func(m_arg); };

  public:

    virtual ~EzThread() { wait(); };
    EzThread() {
        m_func = NULL;
    };
    EzThread(void(*func)(TYPE), TYPE arg) {
        m_func = NULL;
        EZ_MEM_BARRIER();
        EzThread<TYPE>::run(func, arg);
    };
    virtual int run(void(*func)(TYPE), TYPE arg) {
        if ( (func != NULL) && ((EzThreadBase::status() & 0x7) == 0) ) {
            m_func = func;
            m_arg = arg;
            EZ_MEM_BARRIER();
            return EzThreadBase::run();
        }
        return -1;
    };
    virtual int rerun() {
        if ( (m_func != NULL) && ((EzThreadBase::status() & 0x7) == 0) ) {
            return EzThreadBase::run();
        }
        return -1;
    };
    virtual int status() { return EzThreadBase::status(); };
    virtual int wait() { return EzThreadBase::join(); };
};

#endif /* __EZTHREAD_HPP__ */