	/************************************************************************************
 
 Filename    :   Util_SystemInfo_OSX.mm
 Content     :   Various operations to get information about the mac system
 Created     :   October 2, 2014
 
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

#include "Util_SystemInfo.h"

#include <Cocoa/Cocoa.h>

#include <sys/sysctl.h>
#include <sys/types.h>

#include "../Kernel/OVR_String.h"
#include "../Kernel/OVR_System.h"

using namespace OVR;
namespace OVR { namespace Util {

//from http://opensource.apple.com/source/CF/CF-744/CFUtilities.c
OVR::String OSVersionAsString(){

    NSDictionary *systemVersionDictionary =
    [NSDictionary dictionaryWithContentsOfFile:
     @"/System/Library/CoreServices/SystemVersion.plist"];
    
    NSString *systemVersion =
    [systemVersionDictionary objectForKey:@"ProductVersion"];
    return OVR::String([systemVersion UTF8String]);
}

    
//from http://www.starcoder.com/wordpress/2011/10/using-iokit-to-detect-graphics-hardware/
void GetGraphicsCardList(Array< String > &gpus)
{
    // Check the PCI devices for video cards.
    CFMutableDictionaryRef match_dictionary = IOServiceMatching("IOPCIDevice");
    
    // Create a iterator to go through the found devices.
    io_iterator_t entry_iterator;
    
    if (IOServiceGetMatchingServices(kIOMasterPortDefault,
                                     match_dictionary,
                                     &entry_iterator) == kIOReturnSuccess)
    {
        // Actually iterate through the found devices.
        io_registry_entry_t serviceObject;
        while ((serviceObject = IOIteratorNext(entry_iterator)))
        {
            // Put this services object into a dictionary object.
            CFMutableDictionaryRef serviceDictionary;
            if (IORegistryEntryCreateCFProperties(serviceObject,
                                                  &serviceDictionary,
                                                  kCFAllocatorDefault,
                                                  kNilOptions) != kIOReturnSuccess)
            {
                // Failed to create a service dictionary, release and go on.
                IOObjectRelease(serviceObject);
                continue;
            }	
            
            // 		
            // that points to a CFDataRef.
            const void *modelarr = CFDictionaryGetValue(serviceDictionary, CFSTR("model"));
            if (modelarr != nil) {
                if(CFGetTypeID(modelarr) == CFDataGetTypeID())
                {
                    NSData *data = (__bridge NSData*)(CFDataRef)modelarr;
                    NSString *s = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                    gpus.PushBack([s UTF8String]);
                }
            }
            
            // Release the dictionary created by IORegistryEntryCreateCFProperties.
            CFRelease(serviceDictionary);
            
            // Release the serviceObject returned by IOIteratorNext.
            IOObjectRelease(serviceObject);
        }
        
        // Release the entry_iterator created by IOServiceGetMatchingServices.
        IOObjectRelease(entry_iterator);
    }
}
    
} } // namespace OVR { namespace Util {

