/************************************************************************************

Filename    :   OVR_Profile.h
Content     :   Structs and functions for loading and storing device profile settings
Created     :   February 14, 2013
Notes       :
   Profiles are used to store per-user settings that can be transferred and used
   across multiple applications.  For example, player IPD can be configured once 
   and reused for a unified experience across games.  Configuration and saving of profiles
   can be accomplished in game via the Profile API or by the official Oculus Configuration
   Utility.

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

#ifndef OVR_Profile_h
#define OVR_Profile_h

#include "OVR_CAPI_Keys.h"

#include "Sensors/OVR_DeviceConstants.h"
#include "Kernel/OVR_String.h"
#include "Kernel/OVR_RefCount.h"
#include "Kernel/OVR_Array.h"
#include "Kernel/OVR_StringHash.h"
#include "Kernel/OVR_System.h"

namespace OVR {

class HMDInfo; // Opaque forward declaration
class Profile;
class JSON;


// Device key for looking up profiles
struct ProfileDeviceKey
{
    ProfileDeviceKey(const HMDInfo* info);

	// Initialized properly?
	bool Valid;

    // The HMD type
    HmdTypeEnum HmdType;

	// This is the 12 character serial number printed on the HMD
	String PrintedSerial;

	// This is the product name string of the USB sensor device
	// Note: It has been modified from the original to remove spaces and strip off "Oculus"
	String ProductName;

	// This is the product id from the HID info of the USB sensor device
	unsigned ProductId;

    static String SanitizeProductName(String productName);
};


// -----------------------------------------------------------------------------
// ***** ProfileManager

// Profiles are interfaced through a ProfileManager object.  Applications should
// create a ProfileManager each time they intend to read or write user profile data.
// The scope of the ProfileManager object defines when disk I/O is performed.  Disk
// reads are performed on the first profile access and disk writes are performed when
// the ProfileManager goes out of scope.  All profile interactions between these times
// are performed in local memory and are fast.  A typical profile interaction might
// look like this:
//
// {
//     Ptr<ProfileManager> pm      = *ProfileManager::Create();
//     Ptr<Profile>        profile = pm->LoadProfile(Profile_RiftDK1,
//                                                   pm->GetDefaultProfileName(Profile_RiftDK1));
//     if (profile)
//     {   // Retrieve the current profile settings
//     }
// }   // Profile will be destroyed and any disk I/O completed when going out of scope
class ProfileManager : public NewOverrideBase, public SystemSingletonBase<ProfileManager>
{
    friend class OVR::SystemSingletonBase<ProfileManager>;

protected:
    ProfileManager(bool sys_register);
    virtual ~ProfileManager();
    virtual void OnSystemDestroy();

protected:
    // Synchronize ProfileManager access since it may be accessed from multiple threads,
    // as it's shared through DeviceManager.
    Lock                ProfileLock;
    Ptr<JSON>           ProfileCache;
    bool                Changed;
    String              TempBuff;
    String              BasePath;
    
public:
    // In the service process it is important to set the base path because this cannot be detected automatically
    void                SetBasePath(String basePath);

    int                 GetUserCount();
    const char*         GetUser(unsigned int index);
    bool                CreateUser(const char* user, const char* name);
	bool				HasUser(const char* user);
    bool                RemoveUser(const char* user);
    const char*         GetDefaultUser(const ProfileDeviceKey& deviceKey);
    bool                SetDefaultUser(const ProfileDeviceKey& deviceKey, const char* user);

    virtual Profile*    CreateProfile();
    Profile*            GetProfile(const ProfileDeviceKey& deviceKey, const char* user);
    Profile*            GetDefaultUserProfile(const ProfileDeviceKey& deviceKey);
    Profile*            GetDefaultProfile(HmdTypeEnum device);
    Profile*            GetTaggedProfile(const char** key_names, const char** keys, int num_keys);
    bool                SetTaggedProfile(const char** key_names, const char** keys, int num_keys, Profile* profile);
    
    // Force re-reading the settings
    void                Read();

protected:
    // Force writing the settings
    void                ClearProfileData();
    void                Save();

    String              GetProfilePath();
    void                LoadCache(bool create);
    void                LoadV1Profiles(JSON* v1);
    const char*         GetDefaultUser(const char* product, const char* serial);
};


//-------------------------------------------------------------------
// ***** Profile

// The base profile for all users.  This object is not created directly.
// Instead derived device objects provide add specific device members to 
// the base profile
class Profile : public RefCountBase<Profile>
{
protected:
    OVR::Hash<String, JSON*, String::HashFunctor>   ValMap;
    OVR::Array<JSON*>   Values;  
    OVR::String         TempVal;
    String              BasePath;

public:
    ~Profile();

    int                 GetNumValues(const char* key) const;
    const char*         GetValue(const char* key);
    char*               GetValue(const char* key, char* val, int val_length) const;
    bool                GetBoolValue(const char* key, bool default_val) const;
    int                 GetIntValue(const char* key, int default_val) const;
    float               GetFloatValue(const char* key, float default_val) const;
    int                 GetFloatValues(const char* key, float* values, int num_vals) const;
    double              GetDoubleValue(const char* key, double default_val) const;
    int                 GetDoubleValues(const char* key, double* values, int num_vals) const;

    void                SetValue(const char* key, const char* val);
    void                SetBoolValue(const char* key, bool val);
    void                SetIntValue(const char* key, int val);
    void                SetFloatValue(const char* key, float val);
    void                SetFloatValues(const char* key, const float* vals, int num_vals);
    void                SetDoubleValue(const char* key, double val);
    void                SetDoubleValues(const char* key, const double* vals, int num_vals);

    bool                IsDefaultProfile();
    
    bool Close();

protected:
	Profile(String basePath) :
		BasePath(basePath)
	{
	}
    
    void                SetValue(JSON* val);

	static bool         LoadProfile(const ProfileDeviceKey& deviceKey,
                                    const char* user,
                                    Profile** profile);
    void                CopyItems(JSON* root, String prefix);
    
    bool                LoadDeviceFile(unsigned int device_id, const char* serial);
	bool                LoadDeviceProfile(const ProfileDeviceKey& deviceKey);

    bool                LoadProfile(JSON* root,
                                    const char* user,
                                    const char* device_model,
                                    const char* device_serial);

    bool                LoadUser(JSON* root,
                                 const char* user,
                                 const char* device_name,
                                 const char* device_serial);

    friend class ProfileManager;
    friend class WProfileManager;
};

// This path should be passed into the ProfileManager
String GetBaseOVRPath(bool create_dir);


} // namespace OVR

#endif // OVR_Profile_h
