/************************************************************************************

PublicHeader:   None
Filename    :   OVR_Profile.cpp
Content     :   Structs and functions for loading and storing device profile settings
Created     :   February 14, 2013
Notes       :
   
   Profiles are used to store per-user settings that can be transferred and used
   across multiple applications.  For example, player IPD can be configured once 
   and reused for a unified experience across games.  Configuration and saving of profiles
   can be accomplished in game via the Profile API or by the official Oculus Configuration
   Utility.

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

#include "OVR_Profile.h"
#include "OVR_JSON.h"
#include "Kernel/OVR_SysFile.h"
#include "Kernel/OVR_Allocator.h"
#include "OVR_Stereo.h"

#ifdef OVR_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#else
#include <dirent.h>
#include <sys/stat.h>

#ifdef OVR_OS_LINUX
#include <unistd.h>
#include <pwd.h>
#endif

#endif

#define PROFILE_VERSION 2.0
#define MAX_PROFILE_MAJOR_VERSION 2
#define MAX_DEVICE_PROFILE_MAJOR_VERSION 1


namespace OVR {


//-----------------------------------------------------------------------------
// ProfileDeviceKey

ProfileDeviceKey::ProfileDeviceKey(const HMDInfo* info) :
    Valid(false)
{
    if (info)
    {
        PrintedSerial = info->PrintedSerial;
        ProductName = SanitizeProductName(info->ProductName);
        ProductId = info->ProductId;
        HmdType = info->HmdType;

        if (ProductId != 0)
        {
            Valid = true;
        }
    }
}

String ProfileDeviceKey::SanitizeProductName(String productName)
{
    String result;

    if (!productName.IsEmpty())
    {
        const char* product_name = productName.ToCStr();

        // First strip off "Oculus"
        const char* oculus = strstr(product_name, "Oculus ");
        if (oculus)
        {
            product_name = oculus + OVR_strlen("Oculus ");
        }

        // And remove spaces from the name
        for (const char* s = product_name; *s != 0; s++)
        {
            if (*s != ' ')
            {
                result.AppendChar(*s);
            }
        }
    }

    return result;
}



//-----------------------------------------------------------------------------
// Returns the pathname of the JSON file containing the stored profiles
String GetBaseOVRPath(bool create_dir)
{
    String path;

#if defined(OVR_OS_WIN32)

    TCHAR data_path[MAX_PATH];
    SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, NULL, 0, data_path);
    path = String(data_path);
    
    path += "/Oculus";

    if (create_dir)
    {   // Create the Oculus directory if it doesn't exist
        WCHAR wpath[128];
        OVR::UTF8Util::DecodeString(wpath, path.ToCStr());

        DWORD attrib = GetFileAttributes(wpath);
        bool exists = attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
        if (!exists)
        {   
            CreateDirectory(wpath, NULL);
        }
    }

#elif defined(OVR_OS_MAC)

    const char* home = getenv("HOME");
    path = home;
    path += "/Library/Preferences/Oculus";

    if (create_dir)
    {   // Create the Oculus directory if it doesn't exist
        DIR* dir = opendir(path);
        if (dir == NULL)
        {
            mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
        }
        else
        {
            closedir(dir);
        }
    }

#else

    const char* home = getenv("HOME");
    path = home;
    path += "/.config/Oculus";

    if (create_dir)
    {   // Create the Oculus directory if it doesn't exist
        DIR* dir = opendir(path);
        if (dir == NULL)
        {
            mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
        }
        else
        {
            closedir(dir);
        }
    }

#endif

    return path;
}

String ProfileManager::GetProfilePath()
{
    return BasePath + "/ProfileDB.json";
}

static JSON* FindTaggedData(JSON* data, const char** tag_names, const char** qtags, int num_qtags)
{
    if (data == NULL || !(data->Name == "TaggedData") || data->Type != JSON_Array)
        return NULL;

    JSON* tagged_item = data->GetFirstItem();
    while (tagged_item)
    {
        JSON* tags = tagged_item->GetItemByName("tags");
        if (tags->Type == JSON_Array && num_qtags == tags->GetArraySize())
        {   // Check for a full tag match on each item
            int num_matches = 0;
            
            for (int k=0; k<num_qtags; k++)
            {
                JSON* tag = tags->GetFirstItem();
                while (tag)
                {
                    JSON* tagval = tag->GetFirstItem();
                    if (tagval && tagval->Name == tag_names[k])
                    {
                        if (tagval->Value == qtags[k])
                            num_matches++;
                        break;
                    }
                    tag = tags->GetNextItem(tag);
                }
            }

            // if all tags were matched then copy the values into this Profile
            if (num_matches == num_qtags)
            {
                JSON* vals = tagged_item->GetItemByName("vals");
                return vals;
            }
        }

        tagged_item = data->GetNextItem(tagged_item);
    }

    return NULL;
}

static void FilterTaggedData(JSON* data, const char* tag_name, const char* qtag, Array<JSON*>& items)
{
    if (data == NULL || !(data->Name == "TaggedData") || data->Type != JSON_Array)
        return;

    JSON* tagged_item = data->GetFirstItem();
    while (tagged_item)
    {
        JSON* tags = tagged_item->GetItemByName("tags");
        if (tags->Type == JSON_Array)
        {   // Check for a tag match on the requested tag
            
            JSON* tag = tags->GetFirstItem();
            while (tag)
            {
                JSON* tagval = tag->GetFirstItem();
                if (tagval && tagval->Name == tag_name)
                {
                    if (tagval->Value == qtag)
                    {   // Add this item to the output list
                        items.PushBack(tagged_item);
                    }
                    break;
                }
                tag = tags->GetNextItem(tag);
            }
        }

        tagged_item = data->GetNextItem(tagged_item);
    }
}


//-----------------------------------------------------------------------------
// ***** ProfileManager

template<> ProfileManager* OVR::SystemSingletonBase<ProfileManager>::SlowGetInstance()
{
    static OVR::Lock lock;
    OVR::Lock::Locker locker(&lock);
    if (!SingletonInstance) SingletonInstance = new ProfileManager(true);
    return SingletonInstance;
}

ProfileManager::ProfileManager(bool sys_register) :
    Changed(false)
{
    // Attempt to get the base path automatically, but this may fail
    BasePath = GetBaseOVRPath(false);

    if (sys_register)
        PushDestroyCallbacks();
}

ProfileManager::~ProfileManager()
{
    ClearProfileData();
}

void ProfileManager::OnSystemDestroy()
{
    delete this;
}

// In the service process it is important to set the base path because this cannot be detected automatically
void ProfileManager::SetBasePath(String basePath)
{
    if (basePath != BasePath)
    {
        BasePath = basePath;
        LoadCache(false);
    }
}

// Clear the local profile cache
void ProfileManager::ClearProfileData()
{
    Lock::Locker lockScope(&ProfileLock);

    ProfileCache.Clear();
    Changed = false;
}

// Serializes the profiles to disk.
void ProfileManager::Save()
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
        return;

    // Save the profile to disk
    BasePath = GetBaseOVRPath(true);  // create the base directory if it doesn't exist
    String path = GetProfilePath();
    ProfileCache->Save(path);
    Changed = false;
}

// Returns a profile with all system default values
Profile* ProfileManager::GetDefaultProfile(HmdTypeEnum device)
{
    // In the absence of any data, set some reasonable profile defaults.
    // However, this is not future proof and developers should still
    // provide reasonable default values for queried fields.
    
    // Biometric data
    Profile* profile = CreateProfile();
    profile->SetValue(OVR_KEY_USER,               "default");
    profile->SetValue(OVR_KEY_NAME,               "Default");
    profile->SetValue(OVR_KEY_GENDER,             OVR_DEFAULT_GENDER);
    profile->SetFloatValue(OVR_KEY_PLAYER_HEIGHT, OVR_DEFAULT_PLAYER_HEIGHT);
    profile->SetFloatValue(OVR_KEY_EYE_HEIGHT,    OVR_DEFAULT_EYE_HEIGHT);
    profile->SetFloatValue(OVR_KEY_IPD,           OVR_DEFAULT_IPD);
    float half_ipd[2] = { OVR_DEFAULT_IPD / 2, OVR_DEFAULT_IPD / 2 };
    profile->SetFloatValues(OVR_KEY_EYE_TO_NOSE_DISTANCE, half_ipd, 2);
    float dist[2] = {OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL, OVR_DEFAULT_NECK_TO_EYE_VERTICAL};
    profile->SetFloatValues(OVR_KEY_NECK_TO_EYE_DISTANCE, dist, 2);
    
    // Device specific data
    if (device != HmdType_None)
    {
        if (device == HmdType_CrystalCoveProto || device == HmdType_DK2)
        {
            profile->SetValue("EyeCup", "A");
            profile->SetIntValue(OVR_KEY_EYE_RELIEF_DIAL, OVR_DEFAULT_EYE_RELIEF_DIAL);

            // TODO: These defaults are a little bogus and designed for continuity with 0.3
            // eye-relief values.  We need better measurement-based numbers in future releases
            float max_eye_plate[2] = { 0.01965f + 0.018f, 0.01965f + 0.018f };
            profile->SetFloatValues(OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE, max_eye_plate, 2);
        }
        else
        {   // DK1 and DKHD variants
            profile->SetValue("EyeCup", "A");
            profile->SetIntValue(OVR_KEY_EYE_RELIEF_DIAL, OVR_DEFAULT_EYE_RELIEF_DIAL);

            // TODO: These defaults are a little bogus and designed for continuity with 0.3
            // DK1 distortion.  We need better measurement-based numbers in future releases
            float max_eye_plate[2] = { 0.02357f + 0.017f, 0.02357f + 0.017f };
            profile->SetFloatValues(OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE, max_eye_plate, 2);
        }
    }

    return profile;
}

//------------------------------------------------------------------------------
void ProfileManager::Read()
{
    LoadCache(false);
}

// Populates the local profile cache.  This occurs on the first access of the profile
// data.  All profile operations are performed against the local cache until the
// ProfileManager is released or goes out of scope at which time the cache is serialized
// to disk.
void ProfileManager::LoadCache(bool create)
{
    Lock::Locker lockScope(&ProfileLock);

    ClearProfileData();

    String path = GetProfilePath();

    Ptr<JSON> root = *JSON::Load(path);
    if (root == NULL)
    {   
        path = BasePath + "/Profiles.json";  // look for legacy profile
        root = *JSON::Load(path);
        
        if (root == NULL)
        {
            if (create)
            {   // Generate a skeleton profile database
                root = *JSON::CreateObject();
                root->AddNumberItem("Oculus Profile Version", 2.0);
                root->AddItem("Users", JSON::CreateArray());
                root->AddItem("TaggedData", JSON::CreateArray());
                ProfileCache = root;
            }
            
            return;
        }

        // Verify the legacy version
        JSON* version_item = root->GetFirstItem();
        if (version_item->Name == "Oculus Profile Version")
        {
            int major = atoi(version_item->Value.ToCStr());
            if (major != 1)
                return;   // don't use the file on unsupported major version number
        }
        else
        {
            return;      // invalid file
        }

        // Convert the legacy format to the new database format
        LoadV1Profiles(root);
    }
    else
    {
        // Verify the file format and version
        JSON* version_item = root->GetFirstItem();
        if (version_item->Name == "Oculus Profile Version")
        {
            int major = atoi(version_item->Value.ToCStr());
            if (major != 2)
                return;   // don't use the file on unsupported major version number
        }
        else
        {
            return;       // invalid file 
        }

        ProfileCache = root;   // store the database contents for traversal
    }
}

void ProfileManager::LoadV1Profiles(JSON* v1)
{
    JSON* item0 = v1->GetFirstItem();
    JSON* item1 = v1->GetNextItem(item0);
    JSON* item2 = v1->GetNextItem(item1);

    // Create the new profile database
    Ptr<JSON> root = *JSON::CreateObject();
    root->AddNumberItem("Oculus Profile Version", 2.0);
    root->AddItem("Users", JSON::CreateArray());
    root->AddItem("TaggedData", JSON::CreateArray());
    ProfileCache = root;

    const char* default_dk1_user = item1->Value;
    
    // Read the number of profiles
    int   profileCount = (int)item2->dValue;
    JSON* profileItem  = item2;

    for (int p=0; p<profileCount; p++)
    {
        profileItem = root->GetNextItem(profileItem);
        if (profileItem == NULL)
            break;

        if (profileItem->Name == "Profile")
        {
            // Read the required Name field
            const char* profileName;
            JSON* item = profileItem->GetFirstItem();
        
            if (item && (item->Name == "Name"))
            {   
                profileName = item->Value;
            }
            else
            {
                return;   // invalid field
            }
            
            // Read the user profile fields
            if (CreateUser(profileName, profileName))
            {
                const char* tag_names[2] = {"User", "Product"};
                const char* tags[2];
                tags[0] = profileName;

                Ptr<Profile> user_profile = *CreateProfile();
                user_profile->SetValue(OVR_KEY_NAME, profileName);

                float neckeye[2] = { 0, 0 };

                item = profileItem->GetNextItem(item);
                while (item)
                {
                    if (item->Type != JSON_Object)
                    {
                        if (item->Name == OVR_KEY_PLAYER_HEIGHT)
                        {   // Add an explicit eye height

                        }
                        if (item->Name == "NeckEyeHori")
                            neckeye[0] = (float)item->dValue;
                        else if (item->Name == "NeckEyeVert")
                            neckeye[1] = (float)item->dValue;
                        else 
                            user_profile->SetValue(item);
                    }
                    else
                    {   
                        // Add the user/device tag values
                        const char* device_name = item->Name.ToCStr();
                        Ptr<Profile> device_profile = *CreateProfile();

                        JSON* device_item = item->GetFirstItem();
                        while (device_item)
                        {
                            device_profile->SetValue(device_item);
                            device_item = item->GetNextItem(device_item);
                        }

                        tags[1] = device_name;
                        SetTaggedProfile(tag_names, tags, 2, device_profile);
                    }

                    item = profileItem->GetNextItem(item);
                }

                // Add an explicit eye-height field
                float player_height = user_profile->GetFloatValue(OVR_KEY_PLAYER_HEIGHT,
                                                                  OVR_DEFAULT_PLAYER_HEIGHT);
                if (player_height > 0)
                {
                    char gender[16];
                    user_profile->GetValue(OVR_KEY_GENDER, gender, 16);
        
                    const float EYE_TO_HEADTOP_RATIO =   0.44538f;
                    const float MALE_AVG_HEAD_HEIGHT =   0.232f;
                    const float FEMALE_AVG_HEAD_HEIGHT = 0.218f;
     
                    // compute distance from top of skull to the eye
                    float head_height;
                    if (OVR_strcmp(gender, "Female") == 0)
                        head_height = FEMALE_AVG_HEAD_HEIGHT;
                    else
                        head_height = MALE_AVG_HEAD_HEIGHT;

                    float skull = EYE_TO_HEADTOP_RATIO * head_height;
                    float eye_height = player_height - skull;

                    user_profile->SetFloatValue(OVR_KEY_EYE_HEIGHT, eye_height);
                }

                // Convert NeckEye values to an array
                if (neckeye[0] > 0 && neckeye[1] > 0)
                    user_profile->SetFloatValues(OVR_KEY_NECK_TO_EYE_DISTANCE, neckeye, 2);

                // Add the user tag values
                SetTaggedProfile(tag_names, tags, 1, user_profile);
            }
        }
    }

    // since V1 profiles were only for DK1, the assign the user to all DK1's
    const char* tag_names[1] = { "Product" };
    const char* tags[1] = { "RiftDK1" };
    Ptr<Profile> product_profile = *CreateProfile();
    product_profile->SetValue("DefaultUser", default_dk1_user);
    SetTaggedProfile(tag_names, tags, 1, product_profile);
}

// Returns the number of stored profiles for this device type
int ProfileManager::GetUserCount()
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(false);
        if (ProfileCache == NULL)
            return 0;
    }

    JSON* users = ProfileCache->GetItemByName("Users");
    if (users == NULL)
        return 0;

    return users->GetItemCount();
}

bool ProfileManager::CreateUser(const char* user, const char* name)
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(true);
        if (ProfileCache == NULL)
            return false;
    }

    JSON* users = ProfileCache->GetItemByName("Users");
    if (users == NULL)
    {   // Generate the User section
        users = JSON::CreateArray();
        ProfileCache->AddItem("Users", users);
//TODO: Insert this before the TaggedData
    }

    // Search for the pre-existence of this user
    JSON* user_item = users->GetFirstItem();
    int index = 0;
    while (user_item)
    {
        JSON* userid = user_item->GetItemByName("User");
        int compare = OVR_strcmp(user, userid->Value);
        if (compare == 0)
        {   // The user already exists so simply update the fields
            JSON* name_item = user_item->GetItemByName("Name");
            if (name_item && OVR_strcmp(name, name_item->Value) != 0)
            {
                name_item->Value = name;
                Changed = true;
            }
            return true;
        }
        else if (compare < 0)
        {   // A new user should be placed before this item
            break;
        }
        
        user_item = users->GetNextItem(user_item);
        index++;
    }

    // Create and fill the user struct
    JSON* new_user = JSON::CreateObject();
    new_user->AddStringItem(OVR_KEY_USER, user);
    new_user->AddStringItem(OVR_KEY_NAME, name);
    // user_item->AddStringItem("Password", password);

    if (user_item == NULL)
        users->AddArrayElement(new_user);
    else
        users->InsertArrayElement(index, new_user);

    Changed = true;
    return true;
}

// Returns the user id of a specific user in the list.  The returned 
// memory is locally allocated and should not be stored or deleted.  Returns NULL
// if the index is invalid
const char* ProfileManager::GetUser(unsigned int index)
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(false);
        if (ProfileCache == NULL)
            return NULL;
    }

    JSON* users = ProfileCache->GetItemByName("Users");
    
    if (users && index < users->GetItemCount())
    {
        JSON* user_item = users->GetItemByIndex(index);
        if (user_item)
        {
            JSON* user = user_item->GetFirstItem();
            if (user)
            {
                JSON* userid = user_item->GetItemByName(OVR_KEY_USER);
                if (userid)
                    return userid->Value.ToCStr();
            }
        }
    }
    

    return NULL;
}

bool ProfileManager::RemoveUser(const char* user)
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(false);
        if (ProfileCache == NULL)
            return true;
    }

    JSON* users = ProfileCache->GetItemByName("Users");
    if (users == NULL)
        return true;

    // Remove this user from the User table
    JSON* user_item = users->GetFirstItem();
    while (user_item)
    {
        JSON* userid = user_item->GetItemByName("User");
        if (OVR_strcmp(user, userid->Value) == 0)
        {   // Delete the user entry
            user_item->RemoveNode();
            user_item->Release();
            Changed = true;
            break;
        }
        
        user_item = users->GetNextItem(user_item);
    }

    // Now remove all data entries with this user tag
    JSON* tagged_data = ProfileCache->GetItemByName("TaggedData");
    Array<JSON*> user_items;
    FilterTaggedData(tagged_data, "User", user, user_items);
    for (unsigned int i=0; i<user_items.GetSize(); i++)
    {
        user_items[i]->RemoveNode();
        user_items[i]->Release();
        Changed = true;
    }
 
    return Changed;
}

Profile* ProfileManager::CreateProfile()
{
    Profile* profile = new Profile(BasePath);
    return profile;
}

const char* ProfileManager::GetDefaultUser(const ProfileDeviceKey& deviceKey)
{
    const char* product_str = deviceKey.ProductName.IsEmpty() ? NULL : deviceKey.ProductName.ToCStr();
    const char* serial_str = deviceKey.PrintedSerial.IsEmpty() ? NULL : deviceKey.PrintedSerial.ToCStr();

    return GetDefaultUser(product_str, serial_str);
}

// Returns the name of the profile that is marked as the current default user.
const char* ProfileManager::GetDefaultUser(const char* product, const char* serial)
{
    const char* tag_names[2] = {"Product", "Serial"};
    const char* tags[2];

    if (product && serial)
    {
        tags[0] = product;
        tags[1] = serial;
        // Look for a default user on this specific device
        Ptr<Profile> p = *GetTaggedProfile(tag_names, tags, 2);
        if (p == NULL)
        {   // Look for a default user on this product
            p = *GetTaggedProfile(tag_names, tags, 1);
        }

        if (p)
        {   
            const char* user = p->GetValue("DefaultUser");
            if (user != NULL && user[0] != 0)
            {
                TempBuff = user;
                return TempBuff.ToCStr();
            }
        }
    }

    return NULL;
}

//-----------------------------------------------------------------------------
bool ProfileManager::SetDefaultUser(const ProfileDeviceKey& deviceKey, const char* user)
{
    const char* tag_names[2] = {"Product", "Serial"};
    const char* tags[2];

    const char* product_str = deviceKey.ProductName.IsEmpty() ? NULL : deviceKey.ProductName.ToCStr();
    const char* serial_str = deviceKey.PrintedSerial.IsEmpty() ? NULL : deviceKey.PrintedSerial.ToCStr();

    if (product_str && serial_str)
    {
        tags[0] = product_str;
        tags[1] = serial_str;

        Ptr<Profile> p = *CreateProfile();
        p->SetValue("DefaultUser", user);
        return SetTaggedProfile(tag_names, tags, 2, p);
    }

    return false;
}

//-----------------------------------------------------------------------------
Profile* ProfileManager::GetTaggedProfile(const char** tag_names, const char** tags, int num_tags)
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(false);
        if (ProfileCache == NULL)
            return NULL;
    }

    JSON* tagged_data = ProfileCache->GetItemByName("TaggedData");
    OVR_ASSERT(tagged_data);
    if (tagged_data == NULL)
        return NULL;
    
    Profile* profile = new Profile(BasePath);
    
    JSON* vals = FindTaggedData(tagged_data, tag_names, tags, num_tags);
    if (vals)
    {   
        JSON* item = vals->GetFirstItem();
        while (item)
        {
            //printf("Add %s, %s\n", item->Name.ToCStr(), item->Value.ToCStr());
            //profile->Settings.Set(item->Name, item->Value);
            profile->SetValue(item);
            item = vals->GetNextItem(item);
        }

        return profile;
    }
    else
    {
        profile->Release();
        return NULL;
    }
}

//-----------------------------------------------------------------------------
bool ProfileManager::SetTaggedProfile(const char** tag_names, const char** tags, int num_tags, Profile* profile)
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(true);
        if (ProfileCache == NULL)
            return false;  // TODO: Generate a new profile DB
    }

    JSON* tagged_data = ProfileCache->GetItemByName("TaggedData");
    OVR_ASSERT(tagged_data);
    if (tagged_data == NULL)
        return false;

    // Get the cached tagged data section
    JSON* vals = FindTaggedData(tagged_data, tag_names, tags, num_tags);
    if (vals == NULL)
    {  
        JSON* tagged_item = JSON::CreateObject();
        JSON* taglist = JSON::CreateArray();
        for (int i=0; i<num_tags; i++)
        {
            JSON* k = JSON::CreateObject();
            k->AddStringItem(tag_names[i], tags[i]);
            taglist->AddArrayElement(k);
        }

        vals = JSON::CreateObject();
        
        tagged_item->AddItem("tags", taglist);
        tagged_item->AddItem("vals", vals);
        tagged_data->AddArrayElement(tagged_item);
    }

    // Now add or update each profile setting in cache
    for (unsigned int i=0; i<profile->Values.GetSize(); i++)
    {
        JSON* value = profile->Values[i];
        
        bool found = false;
        JSON* item = vals->GetFirstItem();
        while (item)
        {
            if (value->Name == item->Name)
            {
                // Don't allow a pre-existing type to be overridden
                OVR_ASSERT(value->Type == item->Type);

                if (value->Type == item->Type)
                {   // Check for the same value
                    if (value->Type == JSON_Array)
                    {   // Update each array item
                        if (item->GetArraySize() == value->GetArraySize())
                        {   // Update each value (assumed to be basic types and not array of objects)
                            JSON* value_element = value->GetFirstItem();
                            JSON* item_element = item->GetFirstItem();
                            while (item_element && value_element)
                            {
                                if (value_element->Type == JSON_String)
                                {
                                    if (item_element->Value != value_element->Value)
                                    {   // Overwrite the changed value and mark for file update
                                        item_element->Value = value_element->Value;
                                        Changed = true;
                                    }
                                }
                                else {
                                    if (item_element->dValue != value_element->dValue)
                                    {   // Overwrite the changed value and mark for file update
                                        item_element->dValue = value_element->dValue;
                                        Changed = true;
                                    }
                                }
                                
                                value_element = value->GetNextItem(value_element);
                                item_element = item->GetNextItem(item_element);
                            }
                        }
                        else
                        {   // if the array size changed, simply create a new one                            
// TODO: Create the new array
                        }
                    }
                    else if (value->Type == JSON_String)
                    {
                        if (item->Value != value->Value)
                        {   // Overwrite the changed value and mark for file update
                            item->Value = value->Value;
                            Changed = true;
                        }
                    }
                    else {
                        if (item->dValue != value->dValue)
                        {   // Overwrite the changed value and mark for file update
                            item->dValue = value->dValue;
                            Changed = true;
                        }
                    }
                }
                else
                {
                    return false;
                }

                found = true;
                break;
            }
            
            item = vals->GetNextItem(item);
        }

        if (!found)
        {   // Add the new value
            Changed = true;

            if (value->Type == JSON_String)
                vals->AddStringItem(value->Name, value->Value);
            else if (value->Type == JSON_Bool)
                vals->AddBoolItem(value->Name, ((int)value->dValue != 0));
            else if (value->Type == JSON_Number)
                vals->AddNumberItem(value->Name, value->dValue);
            else if (value->Type == JSON_Array)
                vals->AddItem(value->Name, value->Copy());
            else
            {
                OVR_ASSERT(false);
                Changed = false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
Profile* ProfileManager::GetDefaultUserProfile(const ProfileDeviceKey& deviceKey)
{
    const char* userName = GetDefaultUser(deviceKey);

    Profile* profile = GetProfile(deviceKey, userName);

    if (!profile)
    {
        profile = GetDefaultProfile(deviceKey.HmdType);
    }

    return profile;
}

//-----------------------------------------------------------------------------
Profile* ProfileManager::GetProfile(const ProfileDeviceKey& deviceKey, const char* user)
{
    Lock::Locker lockScope(&ProfileLock);

    if (ProfileCache == NULL)
    {   // Load the cache
        LoadCache(false);
        if (ProfileCache == NULL)
            return NULL;
    }
    
    Profile* profile = new Profile(BasePath);

    if (deviceKey.Valid)
    {
        if (!profile->LoadDeviceProfile(deviceKey) && (user == NULL))
        {
            profile->Release();
            return NULL;
        }
    }
    
    if (user)
    {
        const char* product_str = deviceKey.ProductName.IsEmpty() ? NULL : deviceKey.ProductName.ToCStr();
        const char* serial_str = deviceKey.PrintedSerial.IsEmpty() ? NULL : deviceKey.PrintedSerial.ToCStr();

        if (!profile->LoadProfile(ProfileCache.GetPtr(), user, product_str, serial_str))
        {
            profile->Release();
            return NULL;
        }
    }

    return profile;
}


//-----------------------------------------------------------------------------
// ***** Profile

Profile::~Profile()
{
    ValMap.Clear();
    for (unsigned int i=0; i<Values.GetSize(); i++)
        Values[i]->Release();

    Values.Clear();
}

bool Profile::Close()
{
    // TODO:
    return true;
}

//-----------------------------------------------------------------------------
void Profile::CopyItems(JSON* root, String prefix)
{
    JSON* item = root->GetFirstItem();
    while (item)
    {
        String item_name;
        if (prefix.IsEmpty())
            item_name = item->Name;
        else
            item_name = prefix + "." + item->Name;

        if (item->Type == JSON_Object)
        {   // recursively copy the children
            
            CopyItems(item, item_name);
        }
        else
        {
            //Settings.Set(item_name, item->Value);
            SetValue(item);
        }

        item = root->GetNextItem(item);
    }
}

//-----------------------------------------------------------------------------
bool Profile::LoadDeviceFile(unsigned int productId, const char* printedSerialNumber)
{
    if (printedSerialNumber[0] == 0)
        return false;

    String path = BasePath + "/Devices.json";

    // Load the device profiles
    Ptr<JSON> root = *JSON::Load(path);
    if (root == NULL)
        return false;

    // Quick sanity check of the file type and format before we parse it
    JSON* version = root->GetFirstItem();
    if (version && version->Name == "Oculus Device Profile Version")
    {   
        int major = atoi(version->Value.ToCStr());
        if (major > MAX_DEVICE_PROFILE_MAJOR_VERSION)
            return false;   // don't parse the file on unsupported major version number
    }
    else
    {
        return false;
    }   

    JSON* device = root->GetNextItem(version);
    while (device)
    {   
        if (device->Name == "Device")
        {   
            JSON* product_item = device->GetItemByName("ProductID");
            JSON* serial_item = device->GetItemByName("Serial");
            if (product_item && serial_item &&
                (product_item->dValue == productId) && (serial_item->Value == printedSerialNumber))
            {   
                // found the entry for this device so recursively copy all the settings to the profile
                CopyItems(device, "");
                return true;   
            }
        }

        device = root->GetNextItem(device);
    }
    
    return false;
}

#if 0
//-----------------------------------------------------------------------------
static int BCDByte(unsigned int byte)
{
    int digit1 = (byte >> 4) & 0x000f;
    int digit2 = byte & 0x000f;
    int decimal = digit1 * 10 + digit2;
    return decimal;
}
#endif

//-----------------------------------------------------------------------------
bool Profile::LoadDeviceProfile(const ProfileDeviceKey& deviceKey)
{
    bool success = false;
    if (!deviceKey.Valid)
            return false;

#if 0
        int dev_major = BCDByte((sinfo.Version >> 8) & 0x00ff);
        OVR_UNUSED(dev_major);
        //int dev_minor = BCDByte(sinfo.Version & 0xff);
      
        //if (dev_minor > 18)
        //{   // If the firmware supports hardware stored profiles then grab the device profile
            // from the sensor
            // TBD:  Implement this
        //}
        //else
        {
#endif
            // Grab the model and serial number from the device and use it to access the device
            // profile file stored on the local machine
        success = LoadDeviceFile(deviceKey.ProductId, deviceKey.PrintedSerial);
    //}

    return success;
}

//-----------------------------------------------------------------------------
bool Profile::LoadUser(JSON* root, 
                         const char* user,
                          const char* model_name,
                          const char* device_serial)
{
    if (user == NULL)
        return false;

    // For legacy files, convert to old style names
    //if (model_name && OVR_strcmp(model_name, "Oculus Rift DK1") == 0)
    //    model_name = "RiftDK1";
    
    bool user_found = false;
    JSON* data = root->GetItemByName("TaggedData");
    if (data)
    {   
        const char* tag_names[3];
        const char* tags[3];
        tag_names[0] = "User";
        tags[0] = user;
        int num_tags = 1;

        if (model_name)
        {
            tag_names[num_tags] = "Product";
            tags[num_tags] = model_name;
            num_tags++;
        }

        if (device_serial)
        {
            tag_names[num_tags] = "Serial";
            tags[num_tags] = device_serial;
            num_tags++;
        }

        // Retrieve all tag permutations
        for (int combos=1; combos<=num_tags; combos++)
        {
            for (int i=0; i<(num_tags - combos + 1); i++)
            {
                JSON* vals = FindTaggedData(data, tag_names+i, tags+i, combos);
                if (vals)
                {   
                    if (i==0)   // This tag-combination contains a user match
                        user_found = true;

                    // Add the values to the Profile.  More specialized multi-tag values
                    // will take precedence over and overwrite generalized ones 
                    // For example: ("Me","RiftDK1").IPD would overwrite ("Me").IPD
                    JSON* item = vals->GetFirstItem();
                    while (item)
                    {
                        //printf("Add %s, %s\n", item->Name.ToCStr(), item->Value.ToCStr());
                        //Settings.Set(item->Name, item->Value);
                        SetValue(item);
                        item = vals->GetNextItem(item);
                    }
                }
            }
        }
    }

    if (user_found)
        SetValue(OVR_KEY_USER, user);

    return user_found;
}


//-----------------------------------------------------------------------------
bool Profile::LoadProfile(JSON* root,
                          const char* user,
                          const char* device_model,
                          const char* device_serial)
{
    if (!LoadUser(root, user, device_model, device_serial))
        return false;

    return true;
}


//-----------------------------------------------------------------------------
char* Profile::GetValue(const char* key, char* val, int val_length) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value))
    {
        OVR_strcpy(val, val_length, value->Value.ToCStr());
        return val;
    }
    else
    {
        val[0] = 0;
        return NULL;
    }
}

//-----------------------------------------------------------------------------
const char* Profile::GetValue(const char* key)
{
    // Non-reentrant query.  The returned buffer can only be used until the next call
    // to GetValue()
    JSON* value = NULL;
    if (ValMap.Get(key, &value))
    {
        TempVal = value->Value;
        return TempVal.ToCStr();
    }
    else
    {
        return NULL;
    }
}

//-----------------------------------------------------------------------------
int Profile::GetNumValues(const char* key) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value))
    {  
        if (value->Type == JSON_Array)
            return value->GetArraySize();
        else
            return 1;
    }
    else
        return 0;        
}

//-----------------------------------------------------------------------------
bool Profile::GetBoolValue(const char* key, bool default_val) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value) && value->Type == JSON_Bool)
        return (value->dValue != 0);
    else
        return default_val;
}

//-----------------------------------------------------------------------------
int Profile::GetIntValue(const char* key, int default_val) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value) && value->Type == JSON_Number)
        return (int)(value->dValue);
    else
        return default_val;
}

//-----------------------------------------------------------------------------
float Profile::GetFloatValue(const char* key, float default_val) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value) && value->Type == JSON_Number)
        return (float)(value->dValue);
    else
        return default_val;
}

//-----------------------------------------------------------------------------
int Profile::GetFloatValues(const char* key, float* values, int num_vals) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value) && value->Type == JSON_Array)
    {
        int val_count = Alg::Min(value->GetArraySize(), num_vals);
        JSON* item = value->GetFirstItem();
        int count=0;
        while (item && count < val_count)
        {
            if (item->Type == JSON_Number)
                values[count] = (float)item->dValue;
            else
                break;

            count++;
            item = value->GetNextItem(item);
        }

        return count;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
double Profile::GetDoubleValue(const char* key, double default_val) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value) && value->Type == JSON_Number)
        return value->dValue;
    else
        return default_val;
}

//-----------------------------------------------------------------------------
int Profile::GetDoubleValues(const char* key, double* values, int num_vals) const
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value) && value->Type == JSON_Array)
    {
        int val_count = Alg::Min(value->GetArraySize(), num_vals);
        JSON* item = value->GetFirstItem();
        int count=0;
        while (item && count < val_count)
        {
            if (item->Type == JSON_Number)
                values[count] = item->dValue;
            else
                break;

            count++;
            item = value->GetNextItem(item);
        }

        return count;
    }
    return 0;
}

//-----------------------------------------------------------------------------
void Profile::SetValue(JSON* val)
{
    if (val->Type == JSON_Number)
        SetDoubleValue(val->Name, val->dValue);
    else if (val->Type == JSON_Bool)
        SetBoolValue(val->Name, (val->dValue != 0));
    else if (val->Type == JSON_String)
        SetValue(val->Name, val->Value);
    else if (val->Type == JSON_Array)
    {
        if (val == NULL)
            return;

        // Create a copy of the array
        JSON* value = val->Copy();
        Values.PushBack(value);
        ValMap.Set(value->Name, value);
    }
}

//-----------------------------------------------------------------------------
void Profile::SetValue(const char* key, const char* val)
{
    if (key == NULL || val == NULL)
        return;

    JSON* value = NULL;
    if (ValMap.Get(key, &value))
    {
        value->Value = val;
    }
    else
    {
        value = JSON::CreateString(val);
        value->Name = key;

        Values.PushBack(value);
        ValMap.Set(key, value);
    }
}

//-----------------------------------------------------------------------------
void Profile::SetBoolValue(const char* key, bool val)
{
    if (key == NULL)
        return;

    JSON* value = NULL;
    if (ValMap.Get(key, &value))
    {
        value->dValue = val;
    }
    else
    {
        value = JSON::CreateBool(val);
        value->Name = key;

        Values.PushBack(value);
        ValMap.Set(key, value);
    }
}

//-----------------------------------------------------------------------------
void Profile::SetIntValue(const char* key, int val)
{
    SetDoubleValue(key, val);
}

//-----------------------------------------------------------------------------
void Profile::SetFloatValue(const char* key, float val)
{
    SetDoubleValue(key, val);
}

//-----------------------------------------------------------------------------
void Profile::SetFloatValues(const char* key, const float* vals, int num_vals)
{
    JSON* value = NULL;
    int val_count = 0;
    if (ValMap.Get(key, &value))
    {
        if (value->Type == JSON_Array)
        {
            // truncate the existing array if fewer entries provided
            int num_existing_vals = value->GetArraySize();
            for (int i=num_vals; i<num_existing_vals; i++)
                value->RemoveLast();
            
            JSON* item = value->GetFirstItem();
            while (item && val_count < num_vals)
            {
                if (item->Type == JSON_Number)
                    item->dValue = vals[val_count];

                item = value->GetNextItem(item);
                val_count++;
            }
        }
        else
        {
            return;  // Maybe we should change the data type?
        }
    }
    else
    {
        value = JSON::CreateArray();
        value->Name = key;

        Values.PushBack(value);
        ValMap.Set(key, value);
    }

    for (; val_count < num_vals; val_count++)
        value->AddArrayNumber(vals[val_count]);
}

//-----------------------------------------------------------------------------
void Profile::SetDoubleValue(const char* key, double val)
{
    JSON* value = NULL;
    if (ValMap.Get(key, &value))
    {
        value->dValue = val;
    }
    else
    {
        value = JSON::CreateNumber(val);
        value->Name = key;

        Values.PushBack(value);
        ValMap.Set(key, value);
    }
}

//-----------------------------------------------------------------------------
void Profile::SetDoubleValues(const char* key, const double* vals, int num_vals)
{
    JSON* value = NULL;
    int val_count = 0;
    if (ValMap.Get(key, &value))
    {
        if (value->Type == JSON_Array)
        {
            // truncate the existing array if fewer entries provided
            int num_existing_vals = value->GetArraySize();
            for (int i=num_vals; i<num_existing_vals; i++)
                value->RemoveLast();
            
            JSON* item = value->GetFirstItem();
            while (item && val_count < num_vals)
            {
                if (item->Type == JSON_Number)
                    item->dValue = vals[val_count];

                item = value->GetNextItem(item);
                val_count++;
            }
        }
        else
        {
            return;  // Maybe we should change the data type?
        }
    }
    else
    {
        value = JSON::CreateArray();
        value->Name = key;

        Values.PushBack(value);
        ValMap.Set(key, value);
    }

    for (; val_count < num_vals; val_count++)
        value->AddArrayNumber(vals[val_count]);
}

//------------------------------------------------------------------------------
bool Profile::IsDefaultProfile()
{
    return 0 == OVR::String::CompareNoCase("Default", GetValue(OVR_KEY_NAME));
}


}  // namespace OVR
