/************************************************************************************

Filename    :   OVR_Log.cpp
Content     :   Logging support
Created     :   September 19, 2012
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

#include "OVR_Log.h"
#include "OVR_Std.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "../Kernel/OVR_System.h"

#if defined(OVR_OS_MS) && !defined(OVR_OS_MS_MOBILE)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(OVR_OS_ANDROID)
#include <android/log.h>
#elif defined(OVR_OS_LINUX) || defined(OVR_OS_MAC) || defined(OVR_OS_UNIX)
#include <syslog.h>
#endif


class LogSubject : public OVR::SystemSingletonBase<LogSubject>
{
    static bool isShuttingDown;

public:

    LogSubject(){
        isShuttingDown = false;
        // Must be at end of function
        PushDestroyCallbacks();
    }
    
    virtual ~LogSubject(){} // Required because we use delete this below.
    
    virtual void OnThreadDestroy()
    {
        isShuttingDown = true;
    }

    virtual void OnSystemDestroy()
    {
        delete this;
    }

    static bool IsValid() {
        return isShuttingDown == false;
    }

    OVR::Lock logSubjectLock;
    OVR::ObserverScope<OVR::Log::LogHandler> logSubject;
};

bool LogSubject::isShuttingDown;

OVR_DEFINE_SINGLETON(LogSubject);

namespace OVR {

    // Global Log pointer.
    Log* volatile OVR_GlobalLog = 0;

//-----------------------------------------------------------------------------------
// ***** Log Implementation

Log::Log(unsigned logMask) :
    LoggingMask(logMask)
{
#ifdef OVR_OS_WIN32
    hEventSource = RegisterEventSourceA(NULL, "OculusVR");
    OVR_ASSERT(hEventSource != NULL);
#endif
}

Log::~Log()
{
#ifdef OVR_OS_WIN32
    if (hEventSource)
    {
        DeregisterEventSource(hEventSource);
    }
#endif

    // Clear out global log
    if (this == OVR_GlobalLog)
    {
        // TBD: perhaps we should ASSERT if this happens before system shutdown?
        OVR_GlobalLog = 0;
    }
}
void Log::AddLogObserver(ObserverScope<LogHandler> *logObserver)
{
    if (OVR::System::IsInitialized() && LogSubject::GetInstance()->IsValid())
    {
        Lock::Locker locker(&LogSubject::GetInstance()->logSubjectLock);
        logObserver->GetPtr()->Observe(LogSubject::GetInstance()->logSubject);
    }
}
void Log::LogMessageVargInt(LogMessageType messageType, const char* fmt, va_list argList)
{
    if (OVR::System::IsInitialized() && LogSubject::GetInstance()->IsValid())
    {
        // Invoke subject
        char  buffer[MaxLogBufferMessageSize];
        char* pBuffer = buffer;
        char* pAllocated = NULL;

        #if !defined(OVR_CC_MSVC) // Non-Microsoft compilers require you to save a copy of the va_list.
            va_list argListSaved;
            va_copy(argListSaved, argList);
        #endif

        int result = FormatLog(pBuffer, MaxLogBufferMessageSize, Log_Text, fmt, argList);

        if(result >= MaxLogBufferMessageSize) // If there was insufficient capacity...
        {
            pAllocated = (char*)OVR_ALLOC(result + 1); // We assume C++ exceptions are disabled. FormatLog will handle the case that pAllocated is NULL.
            pBuffer = pAllocated;

            #if !defined(OVR_CC_MSVC)
                va_end(argList); // The caller owns argList and will call va_end on it.
                va_copy(argList, argListSaved);
            #endif

            FormatLog(pBuffer, (size_t)result + 1, Log_Text, fmt, argList);
        }

        Lock::Locker locker(&LogSubject::GetInstance()->logSubjectLock);
        LogSubject::GetInstance()->logSubject.GetPtr()->Call(pBuffer, messageType);

        delete[] pAllocated;
    }
}

void Log::LogMessageVarg(LogMessageType messageType, const char* fmt, va_list argList)
{
    if ((messageType & LoggingMask) == 0)
        return;
#ifndef OVR_BUILD_DEBUG
    if (IsDebugMessage(messageType))
        return;
#endif

    char  buffer[MaxLogBufferMessageSize];
    char* pBuffer = buffer;
    char* pAllocated = NULL;

    #if !defined(OVR_CC_MSVC) // Non-Microsoft compilers require you to save a copy of the va_list.
        va_list argListSaved;
        va_copy(argListSaved, argList);
    #endif

    int result = FormatLog(pBuffer, MaxLogBufferMessageSize, messageType, fmt, argList);

    if(result >= MaxLogBufferMessageSize) // If there was insufficient capacity...
    {
        pAllocated = (char*)OVR_ALLOC(result + 1); // We assume C++ exceptions are disabled. FormatLog will handle the case that pAllocated is NULL.
        pBuffer = pAllocated;

        #if !defined(OVR_CC_MSVC)
            va_end(argList); // The caller owns argList and will call va_end on it.
            va_copy(argList, argListSaved);
        #endif

        FormatLog(pBuffer, (size_t)result + 1, messageType, fmt, argList);
    }

    DefaultLogOutput(pBuffer, messageType, result);
    delete[] pAllocated;
}

void OVR::Log::LogMessage(LogMessageType messageType, const char* pfmt, ...)
{
    va_list argList;
    va_start(argList, pfmt);
    LogMessageVarg(messageType, pfmt, argList);
    va_end(argList);
}


// Return behavior is the same as ISO C vsnprintf: returns the required strlen of buffer (which will 
// be >= bufferSize if bufferSize is insufficient) or returns a negative value because the input was bad.
int Log::FormatLog(char* buffer, size_t bufferSize, LogMessageType messageType,
                    const char* fmt, va_list argList)
{
    OVR_ASSERT(buffer && (bufferSize >= 10)); // Need to be able to at least print "Assert: \n" to it.
    if(!buffer || (bufferSize < 10))
        return -1;

    int addNewline = 1;
    int prefixLength = 0;

    switch(messageType)
    {
    case Log_Error:      OVR_strcpy(buffer, bufferSize, "Error: ");  prefixLength = 7; break;
    case Log_Debug:      OVR_strcpy(buffer, bufferSize, "Debug: ");  prefixLength = 7; break;
    case Log_Assert:     OVR_strcpy(buffer, bufferSize, "Assert: "); prefixLength = 8; break;
    case Log_Text:       buffer[0] = 0; addNewline = 0; break;
    case Log_DebugText:  buffer[0] = 0; addNewline = 0; break;
    default:             buffer[0] = 0; addNewline = 0; break;
    }

    char*  buffer2       = buffer + prefixLength;
    size_t size2         = bufferSize - (size_t)prefixLength;
    int    messageLength = OVR_vsnprintf(buffer2, size2, fmt, argList);

    if (addNewline)
    {
        if (messageLength < 0) // If there was a format error... 
        {
            // To consider: append <format error> to the buffer here.
            buffer2[0] = '\n'; // We are guaranteed to have capacity for this.
            buffer2[1] = '\0';
        }
        else
        {
            // If the printed string used all of the capacity or required more than the capacity,
            // Chop the output by one character so we can append the \n safely.
            int messageEnd = (messageLength >= (int)(size2 - 1)) ? (int)(size2 - 2) : messageLength;
            buffer2[messageEnd + 0] = '\n';
            buffer2[messageEnd + 1] = '\0';
        }
    }

    if (messageLength >= 0) // If the format was OK...
        return prefixLength + messageLength + addNewline; // Return the required strlen of buffer.

    return messageLength; // Else we cannot know what the required strlen is and return the error to the caller.
}

void Log::DefaultLogOutput(const char* formattedText, LogMessageType messageType, int bufferSize)
{
    bool debug = IsDebugMessage(messageType);
    OVR_UNUSED(bufferSize);

#if defined(OVR_OS_WIN32)
    // Under Win32, output regular messages to console if it exists; debug window otherwise.
    static DWORD dummyMode;
    static bool  hasConsole = (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE) &&
                              (GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dummyMode));

    if (!hasConsole || debug)
    {
        ::OutputDebugStringA(formattedText);
    }

    fputs(formattedText, stdout);

#elif defined(OVR_OS_MS) // Any other Microsoft OSs

    ::OutputDebugStringA(formattedText);

#elif defined(OVR_OS_ANDROID)
    // To do: use bufferSize to deal with the case that Android has a limited output length.
    __android_log_write(ANDROID_LOG_INFO, "OVR", formattedText);

#else
    fputs(formattedText, stdout);

#endif

    if (messageType == Log_Error)
    {
#if defined(OVR_OS_WIN32)
        if (!ReportEventA(hEventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, &formattedText, NULL))
        {
            OVR_ASSERT(false);
        }
#elif defined(OVR_OS_MS) // Any other Microsoft OSs
        // TBD
#elif defined(OVR_OS_ANDROID)
        // TBD
#elif defined(OVR_OS_MAC) || defined(OVR_OS_LINUX)
        syslog(LOG_ERR, "%s", formattedText);
#else
        // TBD
#endif
    }

    // Just in case.
    OVR_UNUSED2(formattedText, debug);
}


//static
void Log::SetGlobalLog(Log *log)
{
    OVR_GlobalLog = log;
}
//static
Log* Log::GetGlobalLog()
{
// No global log by default?
//    if (!OVR_GlobalLog)
//        OVR_GlobalLog = GetDefaultLog();
    return OVR_GlobalLog;
}

//static
Log* Log::GetDefaultLog()
{
    // Create default log pointer statically so that it can be used
    // even during startup.
    static Log defaultLog;
    return &defaultLog;
}


//-----------------------------------------------------------------------------------
// ***** Global Logging functions

#if !defined(OVR_CC_MSVC)
// The reason for va_copy is because you can't use va_start twice on Linux
#define OVR_LOG_FUNCTION_IMPL(Name)  \
    void Log##Name(const char* fmt, ...) \
    {                                                                    \
        if (OVR_GlobalLog)                                               \
        {                                                                \
            va_list argList1;                                             \
            va_start(argList1, fmt);                                     \
            va_list argList2;                                             \
            va_copy(argList2, argList1);                                 \
            OVR_GlobalLog->LogMessageVargInt(Log_##Name, fmt, argList2); \
            va_end(argList2);                                             \
            OVR_GlobalLog->LogMessageVarg(Log_##Name, fmt, argList1);    \
            va_end(argList1);                                            \
        }                                                                \
    }
#else
#define OVR_LOG_FUNCTION_IMPL(Name)  \
    void Log##Name(const char* fmt, ...) \
    {                                                                    \
        if (OVR_GlobalLog)                                               \
        {                                                                \
            va_list argList1;                                             \
            va_start(argList1, fmt);                                     \
            OVR_GlobalLog->LogMessageVargInt(Log_##Name, fmt, argList1); \
            OVR_GlobalLog->LogMessageVarg(Log_##Name, fmt, argList1);    \
            va_end(argList1);                                            \
        }                                                                \
    }
#endif // #if !defined(OVR_OS_WIN32)

OVR_LOG_FUNCTION_IMPL(Text)
OVR_LOG_FUNCTION_IMPL(Error)

#ifdef OVR_BUILD_DEBUG
OVR_LOG_FUNCTION_IMPL(DebugText)
OVR_LOG_FUNCTION_IMPL(Debug)
OVR_LOG_FUNCTION_IMPL(Assert)
#endif

} // OVR
