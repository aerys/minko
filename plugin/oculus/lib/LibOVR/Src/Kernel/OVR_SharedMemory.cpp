/************************************************************************************

Filename    :   OVR_SharedMemory.cpp
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

#include "OVR_SharedMemory.h"
#include "OVR_Atomic.h"
#include "OVR_Log.h"
#include "OVR_String.h"
#include "OVR_Array.h"

#if defined(OVR_OS_WIN32) && !defined(OVR_FAKE_SHAREDMEMORY)
#include <Sddl.h> // ConvertStringSecurityDescriptorToSecurityDescriptor
#endif // OVR_OS_WIN32

#if (defined(OVR_OS_LINUX) || defined(OVR_OS_MAC)) && !defined(OVR_FAKE_SHAREDMEMORY)
#include <sys/mman.h> // shm_open(), mmap()
#include <errno.h> // error results for mmap
#include <sys/stat.h> // mode constants
#include <fcntl.h> // O_ constants
#include <unistd.h> // close()
#endif // OVR_OS_LINUX

OVR_DEFINE_SINGLETON(OVR::SharedMemoryFactory);

namespace OVR {


    //// Fake version

#if defined(OVR_FAKE_SHAREDMEMORY)

    class FakeMemoryBlock : public RefCountBase<FakeMemoryBlock>
    {
        String Name;
        char*  Data;
        int    SizeBytes;
        int    References;

    public:
        FakeMemoryBlock(const String& name, int size) :
            Name(name),
            Data(NULL),
            SizeBytes(size),
            References(1)
        {
            Data = new char[SizeBytes];
        }
        ~FakeMemoryBlock()
        {
            delete[] Data;
        }

        bool IsNamed(const String& name)
        {
            return Name.CompareNoCase(name) == 0;
        }
        void* GetData()
        {
            return Data;
        }
        int GetSizeI()
        {
            return SizeBytes;
        }
        void IncrementReferences()
        {
            ++References;
        }
        bool DecrementReferences()
        {
            return --References <= 0;
        }
    };

    class SharedMemoryInternal : public NewOverrideBase
    {
    public:
        void* FileView;
        Ptr<FakeMemoryBlock> Block;

        void Close();

        SharedMemoryInternal(FakeMemoryBlock* block) :
            Block(block)
        {
            FileView = Block->GetData();
        }
        ~SharedMemoryInternal()
        {
            Close();
        }

        static SharedMemoryInternal* CreateSharedMemory(const SharedMemory::OpenParameters& params);
    };


    //// FakeMemoryManager

    class FakeMemoryManager : public NewOverrideBase, public SystemSingletonBase<FakeMemoryManager>
    {
        OVR_DECLARE_SINGLETON(FakeMemoryManager);

        Lock FakeLock;
        Array< Ptr<FakeMemoryBlock> > FakeArray;

    public:
        SharedMemoryInternal* Open(const char *name, int bytes, bool openOnly)
        {
            Lock::Locker locker(&FakeLock);

            const int count = FakeArray.GetSizeI();
            for (int ii = 0; ii < count; ++ii)
            {
                if (FakeArray[ii]->IsNamed(name))
                {
                    FakeArray[ii]->IncrementReferences();
                    return new SharedMemoryInternal(FakeArray[ii]);
                }
            }

            if (openOnly)
            {
                return NULL;
            }

            Ptr<FakeMemoryBlock> data = *new FakeMemoryBlock(name, bytes);
            FakeArray.PushBack(data);
            return new SharedMemoryInternal(data);
        }

        void Free(FakeMemoryBlock* block)
        {
            Lock::Locker locker(&FakeLock);

            const int count = FakeArray.GetSizeI();
            for (int ii = 0; ii < count; ++ii)
            {
                if (FakeArray[ii].GetPtr() == block)
                {
                    // If the reference count hit zero,
                    if (FakeArray[ii]->DecrementReferences())
                    {
                        // Toast
                        FakeArray.RemoveAtUnordered(ii);
                    }
                    break;
                }
            }
        }
    };

    FakeMemoryManager::FakeMemoryManager()
    {
        PushDestroyCallbacks();
    }

    FakeMemoryManager::~FakeMemoryManager()
    {
        OVR_ASSERT(FakeArray.GetSizeI() == 0);
    }

    void FakeMemoryManager::OnSystemDestroy()
    {
        delete this;
    }


} // namespace OVR

OVR_DEFINE_SINGLETON(FakeMemoryManager);

namespace OVR {


void SharedMemoryInternal::Close()
{
	FakeMemoryManager::GetInstance()->Free(Block);
	Block.Clear();
}

SharedMemoryInternal* SharedMemoryInternal::CreateSharedMemory(const SharedMemory::OpenParameters& params)
{
	return FakeMemoryManager::GetInstance()->Open(params.globalName, params.minSizeBytes, params.openMode == SharedMemory::OpenMode_OpenOnly);
}

#endif


//// Windows version

#if defined(OVR_OS_WIN32) && !defined(OVR_FAKE_SHAREDMEMORY)

#pragma comment(lib, "advapi32.lib")

// Hidden implementation class for OS-specific behavior
class SharedMemoryInternal : public NewOverrideBase
{
public:
	HANDLE FileMapping;
	void* FileView;

	SharedMemoryInternal(HANDLE fileMapping, void* fileView) :
		FileMapping(fileMapping),
		FileView(fileView)
	{
	}

	~SharedMemoryInternal()
	{
		// If file view is set,
		if (FileView)
		{
			UnmapViewOfFile(FileView);
			FileView = NULL;
		}

		// If file mapping is set,
		if (FileMapping != NULL)
		{
			CloseHandle(FileMapping);
			FileMapping = NULL;
		}
	}

	static SharedMemoryInternal* DoFileMap(HANDLE hFileMapping, const char* fileName, bool openReadOnly, int minSize);
	static SharedMemoryInternal* AttemptOpenSharedMemory(const char* fileName, int minSize, bool openReadOnly);
	static SharedMemoryInternal* AttemptCreateSharedMemory(const char* fileName, int minSize, bool openReadOnly, bool allowRemoteWrite);
	static SharedMemoryInternal* CreateSharedMemory(const SharedMemory::OpenParameters& params);
};

SharedMemoryInternal* SharedMemoryInternal::DoFileMap(HANDLE hFileMapping, const char* fileName, bool openReadOnly, int minSize)
{
	// Interpret the access mode as a map desired access code
	DWORD mapDesiredAccess = openReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE;

	// Map view of the file to this process
	void* pFileView = MapViewOfFile(hFileMapping, mapDesiredAccess, 0, 0, minSize);

	// If mapping could not be created,
	if (!pFileView)
	{
		CloseHandle(hFileMapping);

		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Unable to map view of file for %s error code = %d", fileName, GetLastError()));
        OVR_UNUSED(fileName);
		return NULL;
	}

	// Create internal representation
	SharedMemoryInternal* pimple = new SharedMemoryInternal(hFileMapping, pFileView);

	// If memory allocation fails,
	if (!pimple)
	{
		UnmapViewOfFile(pFileView);
		CloseHandle(hFileMapping);

		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Out of memory"));
		return NULL;
	}

	return pimple;
}

SharedMemoryInternal* SharedMemoryInternal::AttemptOpenSharedMemory(const char* fileName, int minSize, bool openReadOnly)
{
	// Interpret the access mode as a map desired access code
	DWORD mapDesiredAccess = openReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE;

	// Open file mapping
	HANDLE hFileMapping = OpenFileMappingA(mapDesiredAccess, TRUE, fileName);

	// If file was mapped unsuccessfully,
	if (NULL == hFileMapping)
	{
		OVR_DEBUG_LOG(("[SharedMemory] WARNING: Unable to open file mapping for %s error code = %d (not necessarily bad)", fileName, GetLastError()));
		return NULL;
	}

	// Map the file
	return DoFileMap(hFileMapping, fileName, openReadOnly, minSize);
}

SharedMemoryInternal* SharedMemoryInternal::AttemptCreateSharedMemory(const char* fileName, int minSize, bool openReadOnly, bool allowRemoteWrite)
{
	// Prepare a SECURITY_ATTRIBUTES object
	SECURITY_ATTRIBUTES security;
	ZeroMemory(&security, sizeof(security));
	security.nLength = sizeof(security);

	// Security descriptor by DACL strings:
	// ACE strings grant Allow(A), Object/Contains Inheritance (OICI) of:
	// + Grant All (GA) to System (SY)
	// + Grant All (GA) to Built-in Administrators (BA)
	// + Grant Read-Only (GR) or Read-Write (GWGR) to Interactive Users (IU) - ie. games
	static const char* DACLString_ReadOnly = "D:P(A;OICI;GA;;;SY)(A;OICI;GA;;;BA)(A;OICI;GR;;;IU)";
	static const char* DACLString_ReadWrite = "D:P(A;OICI;GA;;;SY)(A;OICI;GA;;;BA)(A;OICI;GWGR;;;IU)";

	// Select the remote process access mode
	const char* remoteAccessString =
		allowRemoteWrite ? DACLString_ReadWrite : DACLString_ReadOnly;

	// Attempt to convert access string to security attributes
	// Note: This will allocate the security descriptor with LocalAlloc() and must be freed later
	BOOL bConvertOkay = ConvertStringSecurityDescriptorToSecurityDescriptorA(
		remoteAccessString, SDDL_REVISION_1, &security.lpSecurityDescriptor, NULL);

	// If conversion fails,
	if (!bConvertOkay)
	{
		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Unable to convert access string, error code = %d", GetLastError()));
		return NULL;
	}

	// Interpret the access mode as a page protection code
	int pageProtectCode = openReadOnly ? PAGE_READONLY : PAGE_READWRITE;

	// Attempt to create a file mapping
	HANDLE hFileMapping = CreateFileMappingA(INVALID_HANDLE_VALUE,	// From page file
											 &security,				// Security attributes
											 pageProtectCode,		// Read-only?
											 0,						// High word for size = 0
											 minSize,				// Low word for size
											 fileName);				// Name of global shared memory file

	// Free the security descriptor buffer
	LocalFree(security.lpSecurityDescriptor);

	// If mapping could not be created,
	if (NULL == hFileMapping)
	{
		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Unable to create file mapping for %s error code = %d", fileName, GetLastError()));
		return NULL;
	}

#ifndef OVR_ALLOW_CREATE_FILE_MAPPING_IF_EXISTS
	// If the file mapping already exists,
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hFileMapping);

		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: File mapping at %s already exists", fileName));
		return NULL;
	}
#endif

	// Map the file
	return DoFileMap(hFileMapping, fileName, openReadOnly, minSize);
}

SharedMemoryInternal* SharedMemoryInternal::CreateSharedMemory(const SharedMemory::OpenParameters& params)
{
	SharedMemoryInternal* retval = NULL;

	// Construct the file mapping name in a Windows-specific way
	OVR::String fileMappingName = params.globalName;
	const char *fileName = fileMappingName.ToCStr();

	// Is being opened read-only?
	const bool openReadOnly = (params.accessMode == SharedMemory::AccessMode_ReadOnly);

	// Try up to 3 times to reduce low-probability failures:
	static const int ATTEMPTS_MAX = 3;
	for (int attempts = 0; attempts < ATTEMPTS_MAX; ++attempts)
	{
		// If opening should be attempted first,
		if (params.openMode != SharedMemory::OpenMode_CreateOnly)
		{
			// Attempt to open a shared memory map
			retval = AttemptOpenSharedMemory(fileName, params.minSizeBytes, openReadOnly);

			// If successful,
			if (retval)
			{
				// Done!
				break;
			}
		}

		// If creating the shared memory is also acceptable,
		if (params.openMode != SharedMemory::OpenMode_OpenOnly)
		{
			// Interpret create mode
			const bool allowRemoteWrite = (params.remoteMode == SharedMemory::RemoteMode_ReadWrite);

			// Attempt to create a shared memory map
			retval = AttemptCreateSharedMemory(fileName, params.minSizeBytes, openReadOnly, allowRemoteWrite);

			// If successful,
			if (retval)
			{
				// Done!
				break;
			}
		}
	} // Re-attempt create/open

	// Note: On Windows the initial contents of the region are guaranteed to be zero.
	return retval;
}

#endif // OVR_OS_WIN32


#if (defined(OVR_OS_LINUX) || defined(OVR_OS_MAC)) && !defined(OVR_FAKE_SHAREDMEMORY)

// Hidden implementation class for OS-specific behavior
class SharedMemoryInternal
{
public:
	int   FileMapping;
	void* FileView;
    int   FileSize;

	SharedMemoryInternal(int fileMapping, void* fileView, int fileSize) :
		FileMapping(fileMapping),
		FileView(fileView),
        FileSize(fileSize)
	{
	}

	~SharedMemoryInternal()
	{
		// If file view is set,
		if (FileView)
		{
            munmap(FileView, FileSize);
			FileView = MAP_FAILED;
		}

		// If file mapping is set,
		if (FileMapping >= 0)
		{
            close(FileMapping);
			FileMapping = -1;
		}
	}

	static SharedMemoryInternal* DoFileMap(int hFileMapping, const char* fileName, bool openReadOnly, int minSize);
	static SharedMemoryInternal* AttemptOpenSharedMemory(const char* fileName, int minSize, bool openReadOnly);
	static SharedMemoryInternal* AttemptCreateSharedMemory(const char* fileName, int minSize, bool openReadOnly, bool allowRemoteWrite);
	static SharedMemoryInternal* CreateSharedMemory(const SharedMemory::OpenParameters& params);
};

SharedMemoryInternal* SharedMemoryInternal::DoFileMap(int hFileMapping, const char* fileName, bool openReadOnly, int minSize)
{
    // Calculate the required flags based on read/write mode
    int prot = openReadOnly ? PROT_READ : (PROT_READ|PROT_WRITE);

    // Map the file view
    void* pFileView = mmap(NULL, minSize, prot, MAP_SHARED, hFileMapping, 0);

    if (pFileView == MAP_FAILED)
    {
        close(hFileMapping);

		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Unable to map view of file for %s error code = %d", fileName, errno));
        OVR_UNUSED(fileName);
		return NULL;
    }

	// Create internal representation
	SharedMemoryInternal* pimple = new SharedMemoryInternal(hFileMapping, pFileView, minSize);

	// If memory allocation fails,
	if (!pimple)
	{
        munmap(pFileView, minSize);
        close(hFileMapping);

		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Out of memory"));
		return NULL;
	}

	return pimple;
}

SharedMemoryInternal* SharedMemoryInternal::AttemptOpenSharedMemory(const char* fileName, int minSize, bool openReadOnly)
{
    // Calculate permissions and flags based on read/write mode
    int flags = openReadOnly ? O_RDONLY : O_RDWR;
    int perms = openReadOnly ? S_IRUSR : (S_IRUSR | S_IWUSR);

    // Attempt to open the shared memory file
    int hFileMapping = shm_open(fileName, flags, perms);

    // If file was not opened successfully,
    if (hFileMapping < 0)
    {
		OVR_DEBUG_LOG(("[SharedMemory] WARNING: Unable to open file mapping for %s error code = %d (not necessarily bad)", fileName, errno));
		return NULL;
    }

	// Map the file
	return DoFileMap(hFileMapping, fileName, openReadOnly, minSize);
}

SharedMemoryInternal* SharedMemoryInternal::AttemptCreateSharedMemory(const char* fileName, int minSize, bool openReadOnly, bool allowRemoteWrite)
{
    // Create mode
    // Note: Cannot create the shared memory file read-only because then ftruncate() will fail.
    int flags = O_CREAT | O_RDWR;

#ifndef OVR_ALLOW_CREATE_FILE_MAPPING_IF_EXISTS
    // Require exclusive access when creating (seems like a good idea without trying it yet..)
    if (shm_unlink(fileName) < 0)
    {
		OVR_DEBUG_LOG(("[SharedMemory] WARNING: Unable to unlink shared memory file %s error code = %d", fileName, errno));
    }
    flags |= O_EXCL;
#endif

    // Set own read/write permissions
    int perms = openReadOnly ? S_IRUSR : (S_IRUSR|S_IWUSR);

    // Allow other users to read/write the shared memory file
    perms |= allowRemoteWrite ? (S_IWGRP|S_IWOTH|S_IRGRP|S_IROTH) : (S_IRGRP|S_IROTH);

    // Attempt to open the shared memory file
    int hFileMapping = shm_open(fileName, flags, perms);

    // If file was not opened successfully,
    if (hFileMapping < 0)
    {
		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Unable to create file mapping for %s error code = %d", fileName, errno));
		return NULL;
    }

    int truncRes = ftruncate(hFileMapping, minSize);

    // If file was not opened successfully,
    if (truncRes < 0)
    {
        close(hFileMapping);
		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Unable to truncate file for %s to %d error code = %d", fileName, minSize, errno));
		return NULL;
    }

	// Map the file
	return DoFileMap(hFileMapping, fileName, openReadOnly, minSize);
}

SharedMemoryInternal* SharedMemoryInternal::CreateSharedMemory(const SharedMemory::OpenParameters& params)
{
	SharedMemoryInternal* retval = NULL;

	// Construct the file mapping name in a Linux-specific way
	OVR::String fileMappingName = "/";
	fileMappingName += params.globalName;
	const char *fileName = fileMappingName.ToCStr();

	// Is being opened read-only?
	const bool openReadOnly = (params.accessMode == SharedMemory::AccessMode_ReadOnly);

	// Try up to 3 times to reduce low-probability failures:
	static const int ATTEMPTS_MAX = 3;
	for (int attempts = 0; attempts < ATTEMPTS_MAX; ++attempts)
	{
		// If opening should be attempted first,
		if (params.openMode != SharedMemory::OpenMode_CreateOnly)
		{
			// Attempt to open a shared memory map
			retval = AttemptOpenSharedMemory(fileName, params.minSizeBytes, openReadOnly);

			// If successful,
			if (retval)
			{
				// Done!
				break;
			}
		}

		// If creating the shared memory is also acceptable,
		if (params.openMode != SharedMemory::OpenMode_OpenOnly)
		{
            // Interpret create mode
            const bool allowRemoteWrite = (params.remoteMode == SharedMemory::RemoteMode_ReadWrite);

            // Attempt to create a shared memory map
            retval = AttemptCreateSharedMemory(fileName, params.minSizeBytes, openReadOnly, allowRemoteWrite);

            // If successful,
            if (retval)
            {
                // Done!
                break;
            }
		}
	} // Re-attempt create/open

	// Note: On Windows the initial contents of the region are guaranteed to be zero.
	return retval;
}

#endif // OVR_OS_LINUX


//// SharedMemory

SharedMemory::SharedMemory(int size, void* data, SharedMemoryInternal* pInternal) :
	Size(size),
	Data(data),
	Internal(pInternal)
{
}
// Call close when it goes out of scope
SharedMemory::~SharedMemory()
{
	Close();
    delete Internal;
}

void SharedMemory::Close()
{
	if (Internal)
	{
		delete Internal;
		Internal = NULL;
	}
}


//// SharedMemoryFactory

Ptr<SharedMemory> SharedMemoryFactory::Open(const SharedMemory::OpenParameters& params)
{
	Ptr<SharedMemory> retval;

	// If no name specified or no size requested,
	if (!params.globalName || (params.minSizeBytes <= 0))
	{
		OVR_DEBUG_LOG(("[SharedMemory] FAILURE: Invalid parameters to Create()"));
		return NULL;
	}

	OVR_DEBUG_LOG(("[SharedMemory] Creating shared memory region: %s > %d bytes",
		params.globalName, params.minSizeBytes));

	// Attempt to create a shared memory region from the parameters
	SharedMemoryInternal* pInternal = SharedMemoryInternal::CreateSharedMemory(params);

	if (pInternal)
	{
		// Create the wrapper object
		retval = *new SharedMemory(params.minSizeBytes, pInternal->FileView, pInternal);
	}

	return retval;
}

SharedMemoryFactory::SharedMemoryFactory()
{
	OVR_DEBUG_LOG(("[SharedMemory] Creating factory"));

    PushDestroyCallbacks();
}

SharedMemoryFactory::~SharedMemoryFactory()
{
	OVR_DEBUG_LOG(("[SharedMemory] Destroying factory"));
}

void SharedMemoryFactory::OnSystemDestroy()
{
    delete this;
}


} // namespace OVR
