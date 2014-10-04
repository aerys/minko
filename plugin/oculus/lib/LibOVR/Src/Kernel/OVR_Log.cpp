/************************************************************************************

Filename    :   OVR_Log.cpp
Content     :   Logging support
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

#include "OVR_Log.h"
#include "OVR_Std.h"
#include <stdarg.h>
#include <stdio.h>

#if defined(OVR_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(OVR_OS_ANDROID)
#include <android/log.h>
#elif defined(OVR_OS_LINUX) || defined(OVR_OS_MAC)
#include <syslog.h>
#endif

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

void Log::LogMessageVarg(LogMessageType messageType, const char* fmt, va_list argList)
{
    if ((messageType & LoggingMask) == 0)
        return;
#ifndef OVR_BUILD_DEBUG
    if (IsDebugMessage(messageType))
        return;
#endif

    char buffer[MaxLogBufferMessageSize];
    FormatLog(buffer, MaxLogBufferMessageSize, messageType, fmt, argList);
    DefaultLogOutput(buffer, messageType);
}

void OVR::Log::LogMessage(LogMessageType messageType, const char* pfmt, ...)
{
    va_list argList;
    va_start(argList, pfmt);
    LogMessageVarg(messageType, pfmt, argList);
    va_end(argList);
}


void Log::FormatLog(char* buffer, unsigned bufferSize, LogMessageType messageType,
                    const char* fmt, va_list argList)
{    
    bool addNewline = true;

    switch(messageType)
    {
    case Log_Error:         OVR_strcpy(buffer, bufferSize, "Error: ");     break;
    case Log_Debug:         OVR_strcpy(buffer, bufferSize, "Debug: ");     break;
    case Log_Assert:        OVR_strcpy(buffer, bufferSize, "Assert: ");    break;
    case Log_Text:       buffer[0] = 0; addNewline = false; break;
    case Log_DebugText:  buffer[0] = 0; addNewline = false; break;
    default:        
        buffer[0] = 0;
        addNewline = false;
        break;
    }

    size_t prefixLength = OVR_strlen(buffer);
    char *buffer2      = buffer + prefixLength;
    OVR_vsprintf(buffer2, bufferSize - prefixLength, fmt, argList);

    if (addNewline)
        OVR_strcat(buffer, bufferSize, "\n");
}


void Log::DefaultLogOutput(const char* formattedText, LogMessageType messageType)
{
    bool debug = IsDebugMessage(messageType);

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

#elif defined(OVR_OS_ANDROID)
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

#define OVR_LOG_FUNCTION_IMPL(Name)  \
    void Log##Name(const char* fmt, ...) \
    {                                                                    \
        if (OVR_GlobalLog)                                               \
        {                                                                \
            va_list argList; va_start(argList, fmt);                     \
            OVR_GlobalLog->LogMessageVarg(Log_##Name, fmt, argList);  \
            va_end(argList);                                             \
        }                                                                \
    }

OVR_LOG_FUNCTION_IMPL(Text)
OVR_LOG_FUNCTION_IMPL(Error)

#ifdef OVR_BUILD_DEBUG
OVR_LOG_FUNCTION_IMPL(DebugText)
OVR_LOG_FUNCTION_IMPL(Debug)
OVR_LOG_FUNCTION_IMPL(Assert)
#endif

} // OVR
