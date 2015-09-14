/************************************************************************************

Filename    :   OVR_ThreadsPthread.cpp
Content     :   
Created     :   
Notes       : 

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_Threads.h"
#include "OVR_Hash.h"

#ifdef OVR_ENABLE_THREADS

#include "OVR_Timer.h"
#include "OVR_Log.h"

#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#if defined(OVR_OS_MAC) || defined(OVR_OS_BSD)
    #include <sys/sysctl.h>
    #include <sys/param.h>
    #if !defined(OVR_OS_MAC)
        #include <pthread_np.h>
    #endif
#endif
    


namespace OVR {

// ***** Mutex implementation


// *** Internal Mutex implementation structure

class MutexImpl : public NewOverrideBase
{
    // System mutex or semaphore
    pthread_mutex_t   SMutex;
    bool          Recursive;
    unsigned      LockCount;
    pthread_t     LockedBy;

    friend class WaitConditionImpl;

public:
    // Constructor/destructor
    MutexImpl(Mutex* pmutex, bool recursive = 1);
    ~MutexImpl();

    // Locking functions
    void                DoLock();
    bool                TryLock();
    void                Unlock(Mutex* pmutex);
    // Returns 1 if the mutes is currently locked
    bool                IsLockedByAnotherThread(Mutex* pmutex);        
    bool                IsSignaled() const;
};

pthread_mutexattr_t Lock::RecursiveAttr;
bool Lock::RecursiveAttrInit = 0;

// *** Constructor/destructor
MutexImpl::MutexImpl(Mutex* pmutex, bool recursive)
{   
    OVR_UNUSED(pmutex);
    Recursive           = recursive;
    LockCount           = 0;

    if (Recursive)
    {
        if (!Lock::RecursiveAttrInit)
        {
            pthread_mutexattr_init(&Lock::RecursiveAttr);
            pthread_mutexattr_settype(&Lock::RecursiveAttr, PTHREAD_MUTEX_RECURSIVE);
            Lock::RecursiveAttrInit = 1;
        }

        pthread_mutex_init(&SMutex, &Lock::RecursiveAttr);
    }
    else
        pthread_mutex_init(&SMutex, 0);
}

MutexImpl::~MutexImpl()
{
    pthread_mutex_destroy(&SMutex);
}


// Lock and try lock
void MutexImpl::DoLock()
{
    while (pthread_mutex_lock(&SMutex))
        ;
    LockCount++;
    LockedBy = pthread_self();
}

bool MutexImpl::TryLock()
{
    if (!pthread_mutex_trylock(&SMutex))
    {
        LockCount++;
        LockedBy = pthread_self();
        return 1;
    }
    
    return 0;
}

void MutexImpl::Unlock(Mutex* pmutex)
{
    OVR_UNUSED(pmutex);
    OVR_ASSERT(pthread_self() == LockedBy && LockCount > 0);

    //unsigned lockCount;
    LockCount--;
    //lockCount = LockCount;

    pthread_mutex_unlock(&SMutex);
}

bool    MutexImpl::IsLockedByAnotherThread(Mutex* pmutex)
{
    OVR_UNUSED(pmutex);
    // There could be multiple interpretations of IsLocked with respect to current thread
    if (LockCount == 0)
        return 0;
    if (pthread_self() != LockedBy)
        return 1;
    return 0;
}

bool    MutexImpl::IsSignaled() const
{
    // An mutex is signaled if it is not locked ANYWHERE
    // Note that this is different from IsLockedByAnotherThread function,
    // that takes current thread into account
    return LockCount == 0;
}


// *** Actual Mutex class implementation

Mutex::Mutex(bool recursive)
{
    // NOTE: RefCount mode already thread-safe for all waitables.
    pImpl = new MutexImpl(this, recursive);
}

Mutex::~Mutex()
{
    delete pImpl;
}

// Lock and try lock
void Mutex::DoLock()
{
    pImpl->DoLock();
}
bool Mutex::TryLock()
{
    return pImpl->TryLock();
}
void Mutex::Unlock()
{
    pImpl->Unlock(this);
}
bool    Mutex::IsLockedByAnotherThread()
{
    return pImpl->IsLockedByAnotherThread(this);
}



//-----------------------------------------------------------------------------------
// ***** Event

bool Event::Wait(unsigned delay)
{
    Mutex::Locker lock(&StateMutex);

    // Do the correct amount of waiting
    if (delay == OVR_WAIT_INFINITE)
    {
        while(!State)
            StateWaitCondition.Wait(&StateMutex);
    }
    else if (delay)
    {
        if (!State)
            StateWaitCondition.Wait(&StateMutex, delay);
    }

    bool state = State;
    // Take care of temporary 'pulsing' of a state
    if (Temporary)
    {
        Temporary   = false;
        State       = false;
    }
    return state;
}

void Event::updateState(bool newState, bool newTemp, bool mustNotify)
{
    Mutex::Locker lock(&StateMutex);
    State       = newState;
    Temporary   = newTemp;
    if (mustNotify)
        StateWaitCondition.NotifyAll();    
}



// ***** Wait Condition Implementation

// Internal implementation class
class WaitConditionImpl : public NewOverrideBase
{
    pthread_mutex_t     SMutex;
    pthread_cond_t      Condv;

public:

    // Constructor/destructor
    WaitConditionImpl();
    ~WaitConditionImpl();

    // Release mutex and wait for condition. The mutex is re-aqured after the wait.
    bool    Wait(Mutex *pmutex, unsigned delay = OVR_WAIT_INFINITE);

    // Notify a condition, releasing at one object waiting
    void    Notify();
    // Notify a condition, releasing all objects waiting
    void    NotifyAll();
};


WaitConditionImpl::WaitConditionImpl()
{
    pthread_mutex_init(&SMutex, 0);
    pthread_cond_init(&Condv, 0);
}

WaitConditionImpl::~WaitConditionImpl()
{
    pthread_mutex_destroy(&SMutex);
    pthread_cond_destroy(&Condv);
}    

bool    WaitConditionImpl::Wait(Mutex *pmutex, unsigned delay)
{
    bool            result = 1;
    unsigned            lockCount = pmutex->pImpl->LockCount;

    // Mutex must have been locked
    if (lockCount == 0)
        return 0;

    pthread_mutex_lock(&SMutex);

    // Finally, release a mutex or semaphore
    if (pmutex->pImpl->Recursive)
    {
        // Release the recursive mutex N times
        pmutex->pImpl->LockCount = 0;
        for(unsigned i=0; i<lockCount; i++)
            pthread_mutex_unlock(&pmutex->pImpl->SMutex);
    }
    else
    {
        pmutex->pImpl->LockCount = 0;
        pthread_mutex_unlock(&pmutex->pImpl->SMutex);
    }

    // Note that there is a gap here between mutex.Unlock() and Wait().
    // The other mutex protects this gap.

    if (delay == OVR_WAIT_INFINITE)
        pthread_cond_wait(&Condv,&SMutex);
    else
    {
        timespec ts;

        struct timeval tv;
        gettimeofday(&tv, 0);

        ts.tv_sec = tv.tv_sec + (delay / 1000);
        ts.tv_nsec = (tv.tv_usec + (delay % 1000) * 1000) * 1000;

        if (ts.tv_nsec > 999999999)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        int r = pthread_cond_timedwait(&Condv,&SMutex, &ts);
        OVR_ASSERT(r == 0 || r == ETIMEDOUT);
        if (r)
            result = 0;
    }

    pthread_mutex_unlock(&SMutex);

    // Re-aquire the mutex
    for(unsigned i=0; i<lockCount; i++)
        pmutex->DoLock(); 

    // Return the result
    return result;
}

// Notify a condition, releasing the least object in a queue
void    WaitConditionImpl::Notify()
{
    pthread_mutex_lock(&SMutex);
    pthread_cond_signal(&Condv);
    pthread_mutex_unlock(&SMutex);
}

// Notify a condition, releasing all objects waiting
void    WaitConditionImpl::NotifyAll()
{
    pthread_mutex_lock(&SMutex);
    pthread_cond_broadcast(&Condv);
    pthread_mutex_unlock(&SMutex);
}



// *** Actual implementation of WaitCondition

WaitCondition::WaitCondition()
{
    pImpl = new WaitConditionImpl;
}
WaitCondition::~WaitCondition()
{
    delete pImpl;
}
    
bool    WaitCondition::Wait(Mutex *pmutex, unsigned delay)
{
    return pImpl->Wait(pmutex, delay);
}
// Notification
void    WaitCondition::Notify()
{
    pImpl->Notify();
}
void    WaitCondition::NotifyAll()
{
    pImpl->NotifyAll();
}


// ***** Current thread

// Per-thread variable
/*
static __thread Thread* pCurrentThread = 0;

// Static function to return a pointer to the current thread
void    Thread::InitCurrentThread(Thread *pthread)
{
    pCurrentThread = pthread;
}

// Static function to return a pointer to the current thread
Thread*    Thread::GetThread()
{
    return pCurrentThread;
}
*/


// *** Thread constructors.

Thread::Thread(UPInt stackSize, int processor)
{
    // NOTE: RefCount mode already thread-safe for all Waitable objects.
    CreateParams params;
    params.stackSize = stackSize;
    params.processor = processor;
    Init(params);
}

Thread::Thread(Thread::ThreadFn threadFunction, void*  userHandle, UPInt stackSize,
                 int processor, Thread::ThreadState initialState)
{
    CreateParams params(threadFunction, userHandle, stackSize, processor, initialState);
    Init(params);
}

Thread::Thread(const CreateParams& params)
{
    Init(params);
}

void Thread::Init(const CreateParams& params)
{
    // Clear the variables    
    ThreadFlags     = 0;
    ThreadHandle    = 0;
    ExitCode        = 0;
    SuspendCount    = 0;
    StackSize       = params.stackSize;
    Processor       = params.processor;
    Priority        = params.priority;

    // Clear Function pointers
    ThreadFunction  = params.threadFunction;
    UserHandle      = params.userHandle;
    if (params.initialState != NotRunning)
        Start(params.initialState);
}

Thread::~Thread()
{
    // Thread should not running while object is being destroyed,
    // this would indicate ref-counting issue.
    //OVR_ASSERT(IsRunning() == 0);

    // Clean up thread.    
    ThreadHandle = 0;
}



// *** Overridable User functions.

// Default Run implementation
int    Thread::Run()
{
    // Call pointer to function, if available.    
    return (ThreadFunction) ? ThreadFunction(this, UserHandle) : 0;
}
void    Thread::OnExit()
{   
}


// Finishes the thread and releases internal reference to it.
void    Thread::FinishAndRelease()
{
    // Note: thread must be US.
    ThreadFlags &= (UInt32)~(OVR_THREAD_STARTED);
    ThreadFlags |= OVR_THREAD_FINISHED;

    // Release our reference; this is equivalent to 'delete this'
    // from the point of view of our thread.
    Release();
}



// *** ThreadList - used to track all created threads

class ThreadList : public NewOverrideBase
{
    //------------------------------------------------------------------------
    struct ThreadHashOp
    {
        size_t operator()(const Thread* ptr)
        {
            return (((size_t)ptr) >> 6) ^ (size_t)ptr;
        }
    };

    HashSet<Thread*, ThreadHashOp>        ThreadSet;
    Mutex                                 ThreadMutex;
    WaitCondition                         ThreadsEmpty;
    // Track the root thread that created us.
    pthread_t                             RootThreadId;

    static ThreadList* volatile pRunningThreads;

    void addThread(Thread *pthread)
    {
        Mutex::Locker lock(&ThreadMutex);
        ThreadSet.Add(pthread);
    }

    void removeThread(Thread *pthread)
    {
        Mutex::Locker lock(&ThreadMutex);
        ThreadSet.Remove(pthread);
        if (ThreadSet.GetSize() == 0)
            ThreadsEmpty.Notify();
    }

    void finishAllThreads()
    {
        // Only original root thread can call this.
        OVR_ASSERT(pthread_self() == RootThreadId);

        Mutex::Locker lock(&ThreadMutex);
        while (ThreadSet.GetSize() != 0)
            ThreadsEmpty.Wait(&ThreadMutex);
    }

public:

    ThreadList()
    {
        RootThreadId = pthread_self();
    }
    ~ThreadList() { }


    static void AddRunningThread(Thread *pthread)
    {
        // Non-atomic creation ok since only the root thread
        if (!pRunningThreads)
        {
            pRunningThreads = new ThreadList;
            OVR_ASSERT(pRunningThreads);
        }
        pRunningThreads->addThread(pthread);
    }

    // NOTE: 'pthread' might be a dead pointer when this is
    // called so it should not be accessed; it is only used
    // for removal.
    static void RemoveRunningThread(Thread *pthread)
    {
        OVR_ASSERT(pRunningThreads);        
        pRunningThreads->removeThread(pthread);
    }

    static void FinishAllThreads()
    {
        // This is ok because only root thread can wait for other thread finish.
        if (pRunningThreads)
        {           
            pRunningThreads->finishAllThreads();
            delete pRunningThreads;
            pRunningThreads = 0;
        }        
    }
};

// By default, we have no thread list.
ThreadList* volatile ThreadList::pRunningThreads = 0;


// FinishAllThreads - exposed publicly in Thread.
void Thread::FinishAllThreads()
{
    ThreadList::FinishAllThreads();
}

// *** Run override

int    Thread::PRun()
{
    // Suspend us on start, if requested
    if (ThreadFlags & OVR_THREAD_START_SUSPENDED)
    {
        Suspend();
        ThreadFlags &= (UInt32)~OVR_THREAD_START_SUSPENDED;
    }

    // Call the virtual run function
    ExitCode = Run();    
    return ExitCode;
}




// *** User overridables

bool    Thread::GetExitFlag() const
{
    return (ThreadFlags & OVR_THREAD_EXIT) != 0;
}       

void    Thread::SetExitFlag(bool exitFlag)
{
    // The below is atomic since ThreadFlags is AtomicInt.
    if (exitFlag)
        ThreadFlags |= OVR_THREAD_EXIT;
    else
        ThreadFlags &= (UInt32) ~OVR_THREAD_EXIT;
}


// Determines whether the thread was running and is now finished
bool    Thread::IsFinished() const
{
    return (ThreadFlags & OVR_THREAD_FINISHED) != 0;
}
// Determines whether the thread is suspended
bool    Thread::IsSuspended() const
{   
    return SuspendCount > 0;
}
// Returns current thread state
Thread::ThreadState Thread::GetThreadState() const
{
    if (IsSuspended())
        return Suspended;    
    if (ThreadFlags & OVR_THREAD_STARTED)
        return Running;    
    return NotRunning;
}

// Join thread
bool Thread::Join(int maxWaitMs) const
{
    // If polling,
    if (maxWaitMs == 0)
    {
        // Just return if finished
        return IsFinished();
    }
    // If waiting forever,
    else if (maxWaitMs > 0)
    {
        UInt32 t0 = Timer::GetTicksMs();

        while (!IsFinished())
        {
            UInt32 t1 = Timer::GetTicksMs();

            // If the wait has expired,
            int delta = (int)(t1 - t0);
            if (delta >= maxWaitMs)
            {
                return false;
            }

            Thread::MSleep(10);
        }

        return true;
    }
    else
    {
        while (!IsFinished())
        {
            pthread_join(ThreadHandle, NULL);
        }
    }

    return true;
}

/*
static const char* mapsched_policy(int policy)
{
    switch(policy)
    {
    case SCHED_OTHER:
        return "SCHED_OTHER";
    case SCHED_RR:
        return "SCHED_RR";
    case SCHED_FIFO:
        return "SCHED_FIFO";

    }
    return "UNKNOWN";
}
    int policy;
    sched_param sparam;
    pthread_getschedparam(pthread_self(), &policy, &sparam);
    int max_prior = sched_get_priority_max(policy);
    int min_prior = sched_get_priority_min(policy);
    printf(" !!!! policy: %s, priority: %d, max priority: %d, min priority: %d\n", mapsched_policy(policy), sparam.sched_priority, max_prior, min_prior);
#include <stdio.h>
*/
// ***** Thread management

// The actual first function called on thread start
void* Thread_PthreadStartFn(void* phandle)
{
    Thread* pthread = (Thread*)phandle;
    int     result = pthread->PRun();
    // Signal the thread as done and release it atomically.
    pthread->FinishAndRelease();
    // At this point Thread object might be dead; however we can still pass
    // it to RemoveRunningThread since it is only used as a key there.   
    ThreadList::RemoveRunningThread(pthread);
    return reinterpret_cast<void*>(result);
}

int Thread::InitAttr = 0;
pthread_attr_t Thread::Attr; 

/* static */
int Thread::GetOSPriority(ThreadPriority p)
{
    OVR_UNUSED(p);
    return -1;
}

/* static */
Thread::ThreadPriority Thread::GetOVRPriority(int osPriority)
{
    #if defined(OVR_OS_LINUX)
        return (ThreadPriority)(Thread::NormalPriority - osPriority); // This works for both SCHED_OTHER, SCHED_RR, and SCHED_FIFO.
    #else
        // Apple priorities are such that the min is a value less than the max.
        static int minPriority = sched_get_priority_min(SCHED_FIFO); // We don't have a means to pass a policy type to this function.
        static int maxPriority = sched_get_priority_max(SCHED_FIFO);

        return (ThreadPriority)(Thread::NormalPriority - (osPriority - ((minPriority + maxPriority) / 2)));
    #endif
}


Thread::ThreadPriority Thread::GetPriority()
{
    int         policy;
    sched_param param;

    int result = pthread_getschedparam(ThreadHandle, &policy, &param);

    if(result == 0)
    {
        #if !defined(OVR_OS_LINUX)
            if(policy == SCHED_OTHER)
            {
                return Thread::NormalPriority; //SCHED_OTHER allows only normal priority on BSD-style Unix and Mac OS X.
            }
        #endif

        return GetOVRPriority(param.sched_priority);
    }

    return Thread::NormalPriority;
}

/* static */
Thread::ThreadPriority Thread::GetCurrentPriority()
{
    int         policy;
    sched_param param;
    pthread_t   currentThreadId = pthread_self();

    int result = pthread_getschedparam(currentThreadId, &policy, &param);

    if(result == 0)
    {
        #if !defined(OVR_OS_LINUX)
            if(policy == SCHED_OTHER)
            {
                return Thread::NormalPriority; //SCHED_OTHER allows only normal priority on BSD-style Unix and Mac OS X.
            }
        #endif

        return GetOVRPriority(param.sched_priority);
    }

    return Thread::NormalPriority;
}


bool Thread::SetPriority(ThreadPriority)
{
    // We currently fail. To do: add code to support this via pthread_getschedparam/pthread_attr_setschedparam
    // This won't work unless using SCHED_FIFO or SCHED_RR anyway, which require root privileges.
    return false;
}

/* static */
bool Thread::SetCurrentPriority(ThreadPriority)
{
    // We currently fail. To do: add code to support this via pthread_getschedparam/pthread_attr_setschedparam
    // This won't work unless using SCHED_FIFO or SCHED_RR anyway, which require root privileges.
    return false;
}

bool    Thread::Start(ThreadState initialState)
{
    if (initialState == NotRunning)
        return 0;
    if (GetThreadState() != NotRunning)
    {
        OVR_DEBUG_LOG(("Thread::Start failed - thread %p already running", this));
        return 0;
    }

    if (!InitAttr)
    {
        pthread_attr_init(&Attr);
        pthread_attr_setdetachstate(&Attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&Attr, 128 * 1024);
        sched_param sparam;
        sparam.sched_priority = Thread::GetOSPriority(NormalPriority);
        pthread_attr_setschedparam(&Attr, &sparam);
        InitAttr = 1;
    }

    ExitCode        = 0;
    SuspendCount    = 0;
    ThreadFlags     = (initialState == Running) ? 0 : OVR_THREAD_START_SUSPENDED;

    // AddRef to us until the thread is finished
    AddRef();
    ThreadList::AddRunningThread(this);

    int result;
    if (StackSize != 128 * 1024 || Priority != NormalPriority)
    {
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&attr, StackSize);
        sched_param sparam;
        sparam.sched_priority = Thread::GetOSPriority(Priority);
        pthread_attr_setschedparam(&attr, &sparam);
        result = pthread_create(&ThreadHandle, &attr, Thread_PthreadStartFn, this);
        pthread_attr_destroy(&attr);
    }
    else
        result = pthread_create(&ThreadHandle, &Attr, Thread_PthreadStartFn, this);

    if (result)
    {
        ThreadFlags = 0;
        Release();
        ThreadList::RemoveRunningThread(this);
        return 0;
    }
    return 1;
}


// Suspend the thread until resumed
bool    Thread::Suspend()
{
    OVR_DEBUG_LOG(("Thread::Suspend - cannot suspend threads on this system"));
    return 0;
}

// Resumes currently suspended thread
bool    Thread::Resume()
{
    return 0;
}


// Quits with an exit code  
void    Thread::Exit(int exitCode)
{
    // Can only exist the current thread
   // if (GetThread() != this)
   //     return;

    // Call the virtual OnExit function
    OnExit();   

    // Signal this thread object as done and release it's references.
    FinishAndRelease();
    ThreadList::RemoveRunningThread(this);

    pthread_exit(reinterpret_cast<void*>(exitCode));
}

ThreadId GetCurrentThreadId()
{
    return (void*)pthread_self();
}

// *** Sleep functions

/* static */
bool    Thread::Sleep(unsigned secs)
{
    sleep(secs);
    return 1;
}
/* static */
bool    Thread::MSleep(unsigned msecs)
{
    usleep(msecs*1000);
    return 1;
}

/* static */
int     Thread::GetCPUCount()
{
    #if defined(OVR_OS_MAC) || defined(OVR_OS_BSD)
        // http://developer.apple.com/mac/library/documentation/Darwin/Reference/ManPages/man3/sysctlbyname.3.html
        int    cpuCount = 0;
        size_t len = sizeof(cpuCount);

        if(sysctlbyname("hw.logicalcpu", &cpuCount, &len, NULL, 0) != 0) 
            cpuCount = 1;

        return cpuCount;

    #else // Linux, Android

        // Alternative: read /proc/cpuinfo
        #ifdef _SC_NPROCESSORS_ONLN
            return (int)sysconf(_SC_NPROCESSORS_ONLN);
        #else
            return 1;
        #endif
    #endif
}


#if defined (OVR_OS_MAC)
void    Thread::SetThreadName( const char* name )
{
    pthread_setname_np( name );
}
#else
void    Thread::SetThreadName( const char* name )
{
    pthread_setname_np( pthread_self(), name );
}
#endif

}

#endif  // OVR_ENABLE_THREADS
