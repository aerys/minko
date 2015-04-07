/************************************************************************************

Filename    :   OVR_OSX_FocusReader.mm
Content     :   Reader for current app with focus on OSX
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

#import "OVR_OSX_FocusReader.h"
#import "OVR_OSX_FocusObserver.h"

@implementation FocusReader

@synthesize window;

- (void) addActivationObserver
{
    //subscribe to focus notifications from the workspace
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                           selector:@selector(activated:)
                                                               name:NSWorkspaceDidActivateApplicationNotification
                                                             object:nil];
    
    //subscribe to focus notifications from the workspace
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


