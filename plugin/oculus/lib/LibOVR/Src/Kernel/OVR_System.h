/************************************************************************************

PublicHeader:   OVR
Filename    :   OVR_System.h
Content     :   General kernel initialization/cleanup, including that
                of the memory allocator.
Created     :   September 19, 2012
Notes       : 

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#ifndef OVR_System_h
#define OVR_System_h

#include "OVR_Allocator.h"
#include "OVR_Log.h"
#include "OVR_Atomic.h"

namespace OVR {


//-----------------------------------------------------------------------------
// SystemSingleton

// Subsystems are implemented using the Singleton pattern.
// To avoid code duplication in all the places where Singletons are defined,
// The pattern is defined once here and used everywhere.

class SystemSingletonInternal
{
    friend class System;

    SystemSingletonInternal* NextSingleton;

    // No copying allowed
    SystemSingletonInternal(const SystemSingletonInternal&) {}
    void operator=(const SystemSingletonInternal&) {}

protected:
    SystemSingletonInternal() :
        NextSingleton(0)
    {
    }

    // Call this to register the destroy events
    // Destroy callbacks will be called in the reverse order they were registered
    // Note: As a rule of thumb, call this at the end of the singleton class constructor.
    void PushDestroyCallbacks();

    // Required: Invoked when the System object is shutting down
    // Called after threads are stopped
    // Called before Log, Allocator, and Timer subsystems are stopped
    // Listeners are called in the opposite order they were registered
    virtual void OnSystemDestroy() = 0;

    // Called just before waiting for threads to die
    // Listeners are called in the opposite order they were registered
    // Useful to start terminating threads at the right time
    // Note: The singleton must not delete itself here.
    virtual void OnThreadDestroy() {}
};

// Singletons derive from this class
template<class T>
class SystemSingletonBase : public SystemSingletonInternal
{
    static AtomicPtr<T> SingletonInstance;
    static T* SlowGetInstance();

protected:
    ~SystemSingletonBase()
    {
        // Make sure the instance gets set to zero on dtor
        if (SingletonInstance == this)
            SingletonInstance = 0;
    }

public:
    static OVR_FORCE_INLINE T* GetInstance()
    {
        // Fast version
        // Note: The singleton instance is stored in an AtomicPtr<> to allow it to be accessed
        // atomically from multiple threads without locks.
        T* instance = SingletonInstance;
        return instance ? instance : SlowGetInstance();
    }
};

// For reference, see N3337 14.5.1.3 (Static data members of class templates):
template<class T> OVR::AtomicPtr<T> OVR::SystemSingletonBase<T>::SingletonInstance;

// Place this in the singleton class in the header file
#define OVR_DECLARE_SINGLETON(T) \
    friend class OVR::SystemSingletonBase<T>; \
private: \
    T(); \
    virtual ~T(); \
    virtual void OnSystemDestroy();

// Place this in the singleton class source file
#define OVR_DEFINE_SINGLETON(T) \
    namespace OVR { \
    template<> T* SystemSingletonBase<T>::SlowGetInstance() \
    { \
        static OVR::Lock lock; \
        OVR::Lock::Locker locker(&lock); \
        if (!SingletonInstance) SingletonInstance = new T; \
        return SingletonInstance; \
    } \
    }


// ***** System Core Initialization class

// System initialization must take place before any other OVR_Kernel objects are used;
// this is done my calling System::Init(). Among other things, this is necessary to
// initialize the memory allocator. Similarly, System::Destroy must be
// called before program exist for proper cleanup. Both of these tasks can be achieved by
// simply creating System object first, allowing its constructor/destructor do the work.

// TBD: Require additional System class for Oculus Rift API?

class System
{
public:
    // System constructor expects allocator to be specified, if it is being substituted.
    System(Log* log = Log::ConfigureDefaultLog(LogMask_Debug),
           Allocator* palloc = DefaultAllocator::InitSystemSingleton())
    {
        Init(log, palloc);
    }
    ~System()
    {
        Destroy();
    }

	static void OVR_CDECL DirectDisplayInitialize();

    // Returns 'true' if system was properly initialized.
    static bool OVR_CDECL IsInitialized();

    // Initializes System core.  Users can override memory implementation by passing
    // a different Allocator here.
    static void OVR_CDECL Init(Log* log = Log::ConfigureDefaultLog(LogMask_Debug),
                               Allocator *palloc = DefaultAllocator::InitSystemSingleton());

	// De-initializes System more, finalizing the threading system and destroying
    // the global memory allocator.
    static void OVR_CDECL Destroy();
};


} // namespace OVR

#endif
