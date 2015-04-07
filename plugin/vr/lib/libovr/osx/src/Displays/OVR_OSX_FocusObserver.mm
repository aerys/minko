/************************************************************************************

Filename    :   OVR_OSX_FocusObserver.mm
Content     :   Observer for app focus on OSX
Created     :   August 5, 2014
Authors     :   Jordan Tritell

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

*************************************************************************************/

OVR_PRIVATE_FILE

#include "OVR_OSX_FocusObserver.h"

#include "../Service/Service_NetServer.h"
#include "OVR_DisplayEnumerator.h"

#include <Cocoa/Cocoa.h>

OVR_DEFINE_SINGLETON(OVR::OSX::AppFocusObserver);

extern bool ServiceRunningFlag;

@interface FocusNotifier : NSObject <NSApplicationDelegate>{
    NSWindow *window;
}

- (void)start;

@property (assign) IBOutlet NSWindow *window;

@end


@implementation FocusNotifier

@synthesize window;

- (void) addActivationObserver
{
    //subscribe to focus notifications from the workspace
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                           selector:@selector(activated:)
                                                               name:NSWorkspaceDidActivateApplicationNotification
                                                             object:nil];
    
    //subscribe to termination notifications from the workspace
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                           selector:@selector(terminated:)
                                                               name:NSWorkspaceDidTerminateApplicationNotification
                                                             object:nil];
}

-(void) activated:(NSNotification *)notification
{
    NSRunningApplication *app = [[notification userInfo] objectForKey:@"NSWorkspaceApplicationKey"];
    pid_t pid = [app processIdentifier];
    OVR::OSX::AppFocusObserver::GetInstance()->OnProcessFocus(pid);
    //    NSLog(@"Activated: %@", [activatedApp bundleIdentifier]);
}

-(void) terminated:(NSNotification *)notification
{
    NSRunningApplication *app = [[notification userInfo] objectForKey:@"NSWorkspaceApplicationKey"];
    pid_t pid = [app processIdentifier];
    OVR::OSX::AppFocusObserver::GetInstance()->RemoveProcess(pid);
    //    NSLog(@"Activated: %@", [activatedApp bundleIdentifier]);
}

- (void)start
{
    //initialize with relevant variables
    [self addActivationObserver];
}

@end


namespace OVR { namespace OSX{

struct FocusNotifierImpl
{
    FocusNotifier* wrapped;
};
    
AppFocusObserver::AppFocusObserver():
    impl(new FocusNotifierImpl),
    listener(NULL)
{
    //initialize with correct values
    impl->wrapped = [[FocusNotifier alloc] init];
    [impl->wrapped start];
    ActiveProcessId = 0;
}

AppFocusObserver::~AppFocusObserver()
{
    [impl->wrapped dealloc];
    delete impl;
}

void AppFocusObserver::SetListener(Service::NetServerListener *_listener)
{
    listener = _listener;
}

void AppFocusObserver::OnSystemDestroy()
{
}

void AppFocusObserver::OnProcessFocus(pid_t pid)
{
    // If the process changed,
    if (pid != LastProcessId)
    {
        LastProcessId = pid;
        
        Lock::Locker locker(&ListLock);
        
        // Find the process id in the list
        const int count = AppList.GetSizeI();
        for (int i = 0; i < count; ++i)
        {
            // If it is a rift process,
            if (AppList[i] == pid)
            {
                onAppFocus(pid);
                OVR_DEBUG_LOG(("[AppFocusObserver] Oculus Process getting focus: pid=%d", pid));
                return;
           }
        }
        
        OVR_DEBUG_LOG(("[AppFocusObserver] Focus change: %d (non-Oculus process)", pid));
    }
}

void AppFocusObserver::AddProcess(pid_t pid)
{
    Lock::Locker locker(&ListLock);
    
    // If it already exists in the array,
    const int count = AppList.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        // If we found it,
        if (AppList[i] == pid)
        {
            return;
        }
    }
    
    // If the process being added is already in focus,
    if (pid == LastProcessId)
    {
        // Set the active process
        OVR_DEBUG_LOG(("[AppFocusObserver] AddProcess: Recognizing the newly added process as in-focus pid=%d", pid));
    }
    
    // Add it to the list
    AppList.PushBack(pid);
}

void AppFocusObserver::nextProcess()
{
    Lock::Locker locker(&ListLock);
    
    int count = AppList.GetSizeI();
    
    // Pick the next available rift process
    if (count > 0)
    {
        ActiveProcessId = AppList[0];
        OVR_DEBUG_LOG(("[AppFocusObserver] NextProcess: Switching active rift process to pid=%d", ActiveProcessId));
        onAppFocus(ActiveProcessId);
        return;
    }
    
    // No process to use
    onAppFocus(ActiveProcessId);
    OVR_DEBUG_LOG(("[AppFocusObserver] NextProcess: No remaining rift processes"));
}


void AppFocusObserver::onAppFocus(pid_t pid)
{
    // Note: This is not necessarily the same as the FocusState->ActiveProcessId.
    
    ActiveProcessId = pid;
    
    if (ActiveProcessId == 0) return;
    
    if (pid != LastAppFocus)
    {
        LastAppFocus = pid;
        if(listener){
            listener->onFocusChange(pid);
        }
//        FocusSubject->Call(pid);
    }
}

void AppFocusObserver::RemoveProcess(pid_t pid)
{
    Lock::Locker locker(&ListLock);
    
    // Find the pid in the app list:
    const int count = AppList.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        // If the app was found,
        if (AppList[i] == pid)
        {            
            // Remove from list here
            AppList.RemoveAtUnordered(i);
            
            // If the removed process is the active one,
            if (ActiveProcessId == pid)
            {
                OVR_DEBUG_LOG(("[AppFocusObserver] RemoveProcess: Active process going away"));
                // Find a new active process
                nextProcess();
            }
            
            break;
        }
    }
}
}} //namespace OVR::OSX

