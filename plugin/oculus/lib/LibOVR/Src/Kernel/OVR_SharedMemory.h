/************************************************************************************

PublicHeader:   OVR
Filename    :   OVR_SharedMemory.h
Content     :   Inter-process shared memory subsystem
Created     :   June 1, 2014
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

#ifndef OVR_SharedMemory_h
#define OVR_SharedMemory_h

#include "OVR_Types.h"
#include "OVR_RefCount.h"
#include "OVR_Allocator.h"
#include "OVR_System.h"

#ifdef OVR_SINGLE_PROCESS /* Everything running in one process usually for debugging */
#define OVR_FAKE_SHAREDMEMORY /* Single-process version to avoid admin privs */
#endif

namespace OVR {

class SharedMemoryInternal; // Opaque


// SharedMemory
// Note: Safe when used between 32-bit and 64-bit processes
class SharedMemory : public RefCountBase<SharedMemory>
{
	friend class SharedMemoryFactory;

	SharedMemory(SharedMemory&) {}
	void operator=(SharedMemory&) {}

public:
	// Only constructed by the SharedMemory Factory
    SharedMemory(int size, void* data, SharedMemoryInternal* pInternal);
	// Call close when it goes out of scope
    ~SharedMemory();

	// Modes for opening a new shared memory region
	enum OpenMode
	{
		// Note: On Windows, Create* requires Administrator priviledges or running as a Service.
		OpenMode_CreateOnly,		// Must not already exist
		OpenMode_OpenOnly,			// Must already exist
		OpenMode_CreateOrOpen		// May exist or not
	};

	// Local access restrictions
	enum AccessMode
	{
		AccessMode_ReadOnly,		// Acquire read-only access
		AccessMode_ReadWrite,		// Acquire read or write access
	};

	// Remote access restrictions
	enum RemoteMode
	{
		RemoteMode_ReadOnly,		// Other processes will need to open in read-only mode
		RemoteMode_ReadWrite		// Other processes can open in read-write mode
	};

	// Modes for opening a new shared memory region
	struct OpenParameters
	{
		OpenParameters() :
			globalName(NULL),
			minSizeBytes(0),
			openMode(SharedMemory::OpenMode_CreateOrOpen),
			remoteMode(SharedMemory::RemoteMode_ReadWrite),
			accessMode(SharedMemory::AccessMode_ReadWrite)
		{
		}

		// Creation parameters
		const char*					globalName;		// Name of the shared memory region
		int							minSizeBytes;	// Minimum number of bytes to request
		SharedMemory::OpenMode		openMode;		// Creating the file or opening the file?
		SharedMemory::RemoteMode	remoteMode;		// When creating, what access should other processes get?
		SharedMemory::AccessMode	accessMode;		// When opening/creating, what access should this process get?
	};

public:
	// Returns the size of the shared memory region
	int GetSizeI() const
	{
		return Size;
	}

	// Returns the process-local pointer to the shared memory region
	// Note: This may be different on different processes
	void* GetData() const
	{
		return Data;
	}

protected:
	int Size;		// How many shared bytes are shared at the pointer address?
	void* Data;		// Pointer to the shared memory region.

	// Hidden implementation class for OS-specific behavior
	SharedMemoryInternal* Internal;

	// Close and cleanup the shared memory region
	// Note: This is called on destruction
	void Close();
};


// SharedMemoryFactory
class SharedMemoryFactory : public NewOverrideBase, public SystemSingletonBase<SharedMemoryFactory>
{
    OVR_DECLARE_SINGLETON(SharedMemoryFactory);

public:
    // Construct a SharedMemory object.
	// Note: The new object is reference-counted so it should be stored with Ptr<>.  Initial reference count is 1.
	Ptr<SharedMemory> Open(const SharedMemory::OpenParameters&);
};


// A shared object
// Its constructor will be called when creating a writer
// Its destructor will not be called
template<class SharedType>
class ISharedObject : public NewOverrideBase
{
public:
	static const int RegionSize = (int)sizeof(SharedType);

protected:
	Ptr<SharedMemory> pSharedMemory;

	bool Open(const char* name, bool readOnly)
	{
		// Configure open parameters based on read-only mode
		SharedMemory::OpenParameters params;

        // FIXME: This is a hack.  We currently need to allow clients to open this for read-write even
        // though they only need read-only access.  This is because in the first 0.4 release the
        // LocklessUpdater class technically writes to it (increments by 0) to read from the space.
        // This was quickly corrected in 0.4.1 and we are waiting for the right time to disallow write
        // access when everyone upgrades to 0.4.1+.
        //params.remoteMode = SharedMemory::RemoteMode_ReadOnly;
        params.remoteMode = SharedMemory::RemoteMode_ReadWrite;

        params.globalName = name;
        params.accessMode = readOnly ? SharedMemory::AccessMode_ReadOnly : SharedMemory::AccessMode_ReadWrite;
        params.minSizeBytes = RegionSize;
		params.openMode = readOnly ? SharedMemory::OpenMode_OpenOnly : SharedMemory::OpenMode_CreateOrOpen;

		// Attempt to open the shared memory file
		pSharedMemory = SharedMemoryFactory::GetInstance()->Open(params);

		// If it was not able to be opened,
		if (pSharedMemory && pSharedMemory->GetSizeI() >= RegionSize && pSharedMemory->GetData())
		{
			// If writing,
			if (!readOnly)
			{
				// Construct the object also
				Construct<SharedType>(pSharedMemory->GetData());
			}

			return true;
		}

		return false;
	}

	SharedType* Get() const
	{
		if (!pSharedMemory)
		{
			return NULL;
		}

		void* data = pSharedMemory->GetData();
		if (!data)
		{
			return NULL;
		}

		return reinterpret_cast<SharedType*>(data);
	}
};

// Writer specialized shared object: Ctor will be called on Open()
template<class SharedType>
class SharedObjectWriter : public ISharedObject<SharedType>
{
public:
	OVR_FORCE_INLINE bool Open(const char* name)
	{
		return ISharedObject<SharedType>::Open(name, false);
	}
	OVR_FORCE_INLINE SharedType* Get()
	{
		return ISharedObject<SharedType>::Get();
	}
};

// Reader specialized shared object: Ctor will not be called
template<class SharedType>
class SharedObjectReader : public ISharedObject<SharedType>
{
public:
	OVR_FORCE_INLINE bool Open(const char* name)
	{
		return ISharedObject<SharedType>::Open(name, true);
	}
	OVR_FORCE_INLINE const SharedType* Get() const
	{
		return ISharedObject<SharedType>::Get();
	}
};


} // namespace OVR

#endif // OVR_SharedMemory_h
