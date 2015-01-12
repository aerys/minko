/************************************************************************************

PublicHeader:   None
Filename    :   OVR_StringHash.h
Content     :   String hash table used when optional case-insensitive
                lookup is required.
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

#ifndef OVR_StringHash_h
#define OVR_StringHash_h

#include "OVR_String.h"
#include "OVR_Hash.h"

namespace OVR {

//-----------------------------------------------------------------------------------
// *** StringHash

// This is a custom string hash table that supports case-insensitive
// searches through special functions such as GetCaseInsensitive, etc.
// This class is used for Flash labels, exports and other case-insensitive tables.

template<class U, class Allocator = ContainerAllocator<U> >
class StringHash : public Hash<String, U, String::NoCaseHashFunctor, Allocator>
{
public:
    typedef U                                                        ValueType;
    typedef StringHash<U, Allocator>                                 SelfType;
    typedef Hash<String, U, String::NoCaseHashFunctor, Allocator>    BaseType;

public:    

    void    operator = (const SelfType& src) { BaseType::operator = (src); }

    bool    GetCaseInsensitive(const String& key, U* pvalue) const
    {
        String::NoCaseKey ikey(key);
        return BaseType::GetAlt(ikey, pvalue);
    }
    // Pointer-returning get variety.
    const U* GetCaseInsensitive(const String& key) const   
    {
        String::NoCaseKey ikey(key);
        return BaseType::GetAlt(ikey);
    }
    U*  GetCaseInsensitive(const String& key)
    {
        String::NoCaseKey ikey(key);
        return BaseType::GetAlt(ikey);
    }

    
    typedef typename BaseType::Iterator base_iterator;

    base_iterator    FindCaseInsensitive(const String& key)
    {
        String::NoCaseKey ikey(key);
        return BaseType::FindAlt(ikey);
    }

    // Set just uses a find and assigns value if found. The key is not modified;
    // this behavior is identical to Flash string variable assignment.    
    void    SetCaseInsensitive(const String& key, const U& value)
    {
        base_iterator it = FindCaseInsensitive(key);
        if (it != BaseType::End())
        {
            it->Second = value;
        }
        else
        {
            BaseType::Add(key, value);
        }
    } 
};

} // OVR 

#endif
