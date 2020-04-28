//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Last modified: 08/17/2008
//
// Filename: src-IL/src/il_alloc.c
//
// Description: Memory allocation functions
//
//-----------------------------------------------------------------------------


#define __ALLOC_C

// Memory leak detection
#ifdef _WIN32
	#ifdef _DEBUG 
		#define _CRTDBG_MAP_ALLOC
		#include <stdlib.h>
		#ifndef _WIN32_WCE  // Does not have this header.
			#include <crtdbg.h>
		#endif
	#endif
#endif//_WIN32

#include "il_internal.h"
#include <stdlib.h>
#include <math.h>

#ifdef MM_MALLOC
#include <mm_malloc.h>
#endif

static void ILAPIENTRY DefaultFreeFunc(const void * CONST_RESTRICT Ptr);
static void* ILAPIENTRY DefaultAllocFunc(const ILsizei Size);

static mAlloc ialloc_ptr = DefaultAllocFunc;
static mFree  ifree_ptr = DefaultFreeFunc;

/*** Vector Allocation/Deallocation Function ***/
#ifdef VECTORMEM
void *vec_malloc(const ILsizei size)
{
	const ILsizei _size =  size % 16 > 0 ? size + 16 - (size % 16) : size; // align size value
	
#ifdef MM_MALLOC
	return _mm_malloc(_size,16);
#else
#ifdef VALLOC
	return valloc( _size );
#else
#ifdef POSIX_MEMALIGN
	char *buffer;
	return posix_memalign((void**)&buffer, 16, _size) == 0 ? buffer : NULL;
#else
#ifdef MEMALIGN
	return memalign( 16, _size );
#else
	// Memalign hack from ffmpeg for MinGW
	void *ptr;
	int diff;
	ptr = malloc(_size+16+1);
	diff= ((-(int)ptr - 1)&15) + 1;
	ptr = (void*)(((char*)ptr)+diff);
	((char*)ptr)[-1]= diff;
	return ptr;
#endif //MEMALIGN
#endif //POSIX_MEMALIGN
#endif //VALLOC
#endif //MM_MALLOC
}

void *ivec_align_buffer(void *buffer, const ILsizei size)
{
	if( (ILsizei)buffer % 16 != 0 ) {
        void *aligned_buffer = vec_malloc( size );
        memcpy( aligned_buffer, buffer, size );
        ifree( buffer );
        return aligned_buffer;
    }
    return buffer;
}
#endif


/*** Allocation/Deallocation Function ***/
void* ILAPIENTRY ialloc(const ILsizei Size)
{
	void *Ptr = ialloc_ptr(Size);
	if (Ptr == NULL)
		ilSetError(IL_OUT_OF_MEMORY);
	return Ptr;
}

void ILAPIENTRY ifree(const void * CONST_RESTRICT Ptr)
{
	if (Ptr == NULL)
		return;
	ifree_ptr(Ptr);
	return;
}

void* ILAPIENTRY icalloc(const ILsizei Size, const ILsizei Num)
{
    void *Ptr = ialloc(Size * Num);
    if (Ptr == NULL)
    	return Ptr;
    imemclear(Ptr, Size * Num);
    return Ptr;
}

/*** Default Allocation/Deallocation Function ***/
static void* ILAPIENTRY DefaultAllocFunc(const ILsizei Size)
{
#ifdef VECTORMEM
	return (void*)vec_malloc(Size);
#else
	return malloc(Size);
#endif //VECTORMEM
}

static void ILAPIENTRY DefaultFreeFunc(const void * CONST_RESTRICT ptr)
{
	if (ptr)
	{
#ifdef MM_MALLOC
	    _mm_free((void*)ptr);
#else
#if defined(VECTORMEM) & !defined(POSIX_MEMALIGN) & !defined(VALLOC) & !defined(MEMALIGN) & !defined(MM_MALLOC)
	    free(((char*)Ptr) - ((char*)Ptr)[-1]);
#else	    
	    free((void*)ptr);
#endif //OTHERS...
#endif //MM_MALLOC
	}
}

/*** Manipulate Allocation/Deallocation Function ***/
void ILAPIENTRY ilResetMemory()  // Deprecated
{
	ialloc_ptr = DefaultAllocFunc;
	ifree_ptr = DefaultFreeFunc;
}

void ILAPIENTRY ilSetMemory(mAlloc AllocFunc, mFree FreeFunc)
{
	ialloc_ptr = AllocFunc == NULL ? DefaultAllocFunc : AllocFunc;
	ifree_ptr = FreeFunc == NULL ? DefaultFreeFunc : FreeFunc;
}


/*#if defined(_WIN32) && defined(_MEM_DEBUG)
#include <windows.h>

int bAtexit = 0;


typedef struct ALLOC_INFO
{
	unsigned long	address;
	unsigned long	size;
	char			file[64];
	unsigned long	line;
	struct ALLOC_INFO *Next;
} ALLOC_INFO;
ALLOC_INFO *AllocList;


void AddTrack(unsigned long addr, unsigned long size, const char *file, unsigned long line)
{
	ALLOC_INFO *Temp;
	
	if (AllocList == NULL) {
		AllocList = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));  // Just assume it succeeds.
		AllocList->address = addr;
		AllocList->size = size;
		AllocList->line = line;
		strncpy(AllocList->file, file, 63);
		AllocList->Next = NULL;
	}
	else {
		Temp = AllocList;
		AllocList = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));  // Just assume it succeeds.
		AllocList->address = addr;
		AllocList->size = size;
		AllocList->line = line;
		strncpy(AllocList->file, file, 63);
		AllocList->Next = Temp;
	}
	
	return;
}


void RemoveTrack(unsigned long addr)
{
	ALLOC_INFO *Temp, *Prev;
	
	Temp = AllocList;
	Prev = NULL;
	
	if (Temp == NULL)
		return;
	
	while (Temp != NULL) {
		if (Temp->address == addr) {
			if (Prev == NULL) {
				AllocList = Temp->Next;
				free(Temp);
			}
			else {
				Prev->Next = Temp->Next;
				free(Temp);
			}
			break;
		}
		Prev = Temp;
		Temp = Temp->Next;
	}
	
	return;
}


void DumpUnfreed(void)
{
	unsigned long TotalSize = 0;
	char buf[1024];
	ALLOC_INFO *i = AllocList;
	
	OutputDebugString("DevIL Unfreed Information:\n");
	while (i != NULL) {
		sprintf(buf, "%s(%d) : %d bytes unfreed at %d\n", i->file, i->line, i->size, i->address);
		OutputDebugString(buf);
		TotalSize += i->size;
		
		AllocList = i->Next;
		free(i);
		i = AllocList;
	}
	
	sprintf(buf, "-----------------------------------------------------------\n");
	OutputDebugString(buf);
	sprintf(buf, "Total Unfreed: %d bytes\n\n\n", TotalSize);
	OutputDebugString(buf);
}

void AddToAtexit()
{
	if (bAtexit)
		return;
	atexit(DumpUnfreed);
	bAtexit = 1;
}

void *c_alloc(unsigned long size, unsigned long num, const char *file, unsigned long line)
{
	void *ptr;
	ptr = calloc(size, num);
	if (!ptr)
		return NULL;
	AddToAtexit();
	AddTrack((unsigned long)ptr, size * num, file, line);
	return ptr;
}


void *m_alloc(unsigned long size, const char *file, unsigned long line)
{
	void *ptr;
	ptr = malloc(size);
	if (!ptr)
		return NULL;
	AddToAtexit();
	AddTrack((unsigned long)ptr, size, file, line);
	return ptr;
}


void f_ree(void *ptr)
{
	RemoveTrack((unsigned long)ptr);
	free(ptr);
	return;
}

#endif//defined(_WIN32) && defined(_MEM_DEBUG)*/
