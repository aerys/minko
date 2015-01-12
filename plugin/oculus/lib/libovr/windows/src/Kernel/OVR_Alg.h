/************************************************************************************

PublicHeader:   OVR_Kernel.h
Filename    :   OVR_Alg.h
Content     :   Simple general purpose algorithms: Sort, Binary Search, etc.
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

#ifndef OVR_Alg_h
#define OVR_Alg_h

#include "OVR_Types.h"
#include <string.h>

namespace OVR { namespace Alg {


//-----------------------------------------------------------------------------------
// ***** Operator extensions

template <typename T> OVR_FORCE_INLINE void Swap(T &a, T &b) 
{  T temp(a); a = b; b = temp; }


// ***** min/max are not implemented in Visual Studio 6 standard STL

template <typename T> OVR_FORCE_INLINE const T Min(const T a, const T b)
{ return (a < b) ? a : b; }

template <typename T> OVR_FORCE_INLINE const T Max(const T a, const T b)
{ return (b < a) ? a : b; }

template <typename T> OVR_FORCE_INLINE const T Clamp(const T v, const T minVal, const T maxVal)
{ return Max<T>(minVal, Min<T>(v, maxVal)); }

template <typename T> OVR_FORCE_INLINE int     Chop(T f)
{ return (int)f; }

template <typename T> OVR_FORCE_INLINE T       Lerp(T a, T b, T f) 
{ return (b - a) * f + a; }


// These functions stand to fix a stupid VC++ warning (with /Wp64 on):
// "warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned', possible loss of data"
// Use these functions instead of gmin/gmax if the argument has size
// of the pointer to avoid the warning. Though, functionally they are
// absolutelly the same as regular gmin/gmax.
template <typename T>   OVR_FORCE_INLINE const T PMin(const T a, const T b)
{
    OVR_COMPILER_ASSERT(sizeof(T) == sizeof(size_t));
    return (a < b) ? a : b;
}
template <typename T>   OVR_FORCE_INLINE const T PMax(const T a, const T b)
{
    OVR_COMPILER_ASSERT(sizeof(T) == sizeof(size_t));
    return (b < a) ? a : b;
}


template <typename T>   OVR_FORCE_INLINE const T Abs(const T v)
{ return (v>=0) ? v : -v; }


//-----------------------------------------------------------------------------------
// ***** OperatorLess
//
template<class T> struct OperatorLess
{
    static bool Compare(const T& a, const T& b)
    {
        return a < b;
    }
};


//-----------------------------------------------------------------------------------
// ***** QuickSortSliced
//
// Sort any part of any array: plain, Array, ArrayPaged, ArrayUnsafe.
// The range is specified with start, end, where "end" is exclusive!
// The comparison predicate must be specified.
template<class Array, class Less> 
void QuickSortSliced(Array& arr, size_t start, size_t end, Less less)
{
    enum 
    {
        Threshold = 9
    };

    if(end - start <  2) return;

    intptr_t  stack[80];
    intptr_t* top   = stack; 
    intptr_t  base  = (intptr_t)start;
    intptr_t  limit = (intptr_t)end;

    for(;;)
    {
        intptr_t len = limit - base;
        intptr_t i, j, pivot;

        if(len > Threshold)
        {
            // we use base + len/2 as the pivot
            pivot = base + len / 2;
            Swap(arr[base], arr[pivot]);

            i = base + 1;
            j = limit - 1;

            // now ensure that *i <= *base <= *j 
            if(less(arr[j],    arr[i])) Swap(arr[j],    arr[i]);
            if(less(arr[base], arr[i])) Swap(arr[base], arr[i]);
            if(less(arr[j], arr[base])) Swap(arr[j], arr[base]);

            for(;;)
            {
                do i++; while( less(arr[i], arr[base]) );
                do j--; while( less(arr[base], arr[j]) );

                if( i > j )
                {
                    break;
                }

                Swap(arr[i], arr[j]);
            }

            Swap(arr[base], arr[j]);

            // now, push the largest sub-array
            if(j - base > limit - i)
            {
                top[0] = base;
                top[1] = j;
                base   = i;
            }
            else
            {
                top[0] = i;
                top[1] = limit;
                limit  = j;
            }
            top += 2;
        }
        else
        {
            // the sub-array is small, perform insertion sort
            j = base;
            i = j + 1;

            for(; i < limit; j = i, i++)
            {
                for(; less(arr[j + 1], arr[j]); j--)
                {
                    Swap(arr[j + 1], arr[j]);
                    if(j == base)
                    {
                        break;
                    }
                }
            }
            if(top > stack)
            {
                top  -= 2;
                base  = top[0];
                limit = top[1];
            }
            else
            {
                break;
            }
        }
    }
}


//-----------------------------------------------------------------------------------
// ***** QuickSortSliced
//
// Sort any part of any array: plain, Array, ArrayPaged, ArrayUnsafe.
// The range is specified with start, end, where "end" is exclusive!
// The data type must have a defined "<" operator.
template<class Array> 
void QuickSortSliced(Array& arr, size_t start, size_t end)
{
    typedef typename Array::ValueType ValueType;
    QuickSortSliced(arr, start, end, OperatorLess<ValueType>::Compare);
}

// Same as corresponding G_QuickSortSliced but with checking array limits to avoid
// crash in the case of wrong comparator functor.
template<class Array, class Less> 
bool QuickSortSlicedSafe(Array& arr, size_t start, size_t end, Less less)
{
    enum 
    {
        Threshold = 9
    };

    if(end - start <  2) return true;

    intptr_t  stack[80];
    intptr_t* top   = stack; 
    intptr_t  base  = (intptr_t)start;
    intptr_t  limit = (intptr_t)end;

    for(;;)
    {
        intptr_t len = limit - base;
        intptr_t i, j, pivot;

        if(len > Threshold)
        {
            // we use base + len/2 as the pivot
            pivot = base + len / 2;
            Swap(arr[base], arr[pivot]);

            i = base + 1;
            j = limit - 1;

            // now ensure that *i <= *base <= *j 
            if(less(arr[j],    arr[i])) Swap(arr[j],    arr[i]);
            if(less(arr[base], arr[i])) Swap(arr[base], arr[i]);
            if(less(arr[j], arr[base])) Swap(arr[j], arr[base]);

            for(;;)
            {
                do 
                {   
                    i++; 
                    if (i >= limit)
                        return false;
                } while( less(arr[i], arr[base]) );
                do 
                {
                    j--; 
                    if (j < 0)
                        return false;
                } while( less(arr[base], arr[j]) );

                if( i > j )
                {
                    break;
                }

                Swap(arr[i], arr[j]);
            }

            Swap(arr[base], arr[j]);

            // now, push the largest sub-array
            if(j - base > limit - i)
            {
                top[0] = base;
                top[1] = j;
                base   = i;
            }
            else
            {
                top[0] = i;
                top[1] = limit;
                limit  = j;
            }
            top += 2;
        }
        else
        {
            // the sub-array is small, perform insertion sort
            j = base;
            i = j + 1;

            for(; i < limit; j = i, i++)
            {
                for(; less(arr[j + 1], arr[j]); j--)
                {
                    Swap(arr[j + 1], arr[j]);
                    if(j == base)
                    {
                        break;
                    }
                }
            }
            if(top > stack)
            {
                top  -= 2;
                base  = top[0];
                limit = top[1];
            }
            else
            {
                break;
            }
        }
    }
    return true;
}

template<class Array> 
bool QuickSortSlicedSafe(Array& arr, size_t start, size_t end)
{
    typedef typename Array::ValueType ValueType;
    return QuickSortSlicedSafe(arr, start, end, OperatorLess<ValueType>::Compare);
}

//-----------------------------------------------------------------------------------
// ***** QuickSort
//
// Sort an array Array, ArrayPaged, ArrayUnsafe.
// The array must have GetSize() function.
// The comparison predicate must be specified.
template<class Array, class Less> 
void QuickSort(Array& arr, Less less)
{
    QuickSortSliced(arr, 0, arr.GetSize(), less);
}

// checks for boundaries
template<class Array, class Less> 
bool QuickSortSafe(Array& arr, Less less)
{
    return QuickSortSlicedSafe(arr, 0, arr.GetSize(), less);
}


//-----------------------------------------------------------------------------------
// ***** QuickSort
//
// Sort an array Array, ArrayPaged, ArrayUnsafe.
// The array must have GetSize() function.
// The data type must have a defined "<" operator.
template<class Array> 
void QuickSort(Array& arr)
{
    typedef typename Array::ValueType ValueType;
    QuickSortSliced(arr, 0, arr.GetSize(), OperatorLess<ValueType>::Compare);
}

template<class Array> 
bool QuickSortSafe(Array& arr)
{
    typedef typename Array::ValueType ValueType;
    return QuickSortSlicedSafe(arr, 0, arr.GetSize(), OperatorLess<ValueType>::Compare);
}

//-----------------------------------------------------------------------------------
// ***** InsertionSortSliced
//
// Sort any part of any array: plain, Array, ArrayPaged, ArrayUnsafe.
// The range is specified with start, end, where "end" is exclusive!
// The comparison predicate must be specified.
// Unlike Quick Sort, the Insertion Sort works much slower in average, 
// but may be much faster on almost sorted arrays. Besides, it guarantees
// that the elements will not be swapped if not necessary. For example, 
// an array with all equal elements will remain "untouched", while 
// Quick Sort will considerably shuffle the elements in this case.
template<class Array, class Less> 
void InsertionSortSliced(Array& arr, size_t start, size_t end, Less less)
{
    size_t j = start;
    size_t i = j + 1;
    size_t limit = end;

    for(; i < limit; j = i, i++)
    {
        for(; less(arr[j + 1], arr[j]); j--)
        {
            Swap(arr[j + 1], arr[j]);
            if(j <= start)
            {
                break;
            }
        }
    }
}


//-----------------------------------------------------------------------------------
// ***** InsertionSortSliced
//
// Sort any part of any array: plain, Array, ArrayPaged, ArrayUnsafe.
// The range is specified with start, end, where "end" is exclusive!
// The data type must have a defined "<" operator.
template<class Array> 
void InsertionSortSliced(Array& arr, size_t start, size_t end)
{
    typedef typename Array::ValueType ValueType;
    InsertionSortSliced(arr, start, end, OperatorLess<ValueType>::Compare);
}

//-----------------------------------------------------------------------------------
// ***** InsertionSort
//
// Sort an array Array, ArrayPaged, ArrayUnsafe.
// The array must have GetSize() function.
// The comparison predicate must be specified.

template<class Array, class Less> 
void InsertionSort(Array& arr, Less less)
{
    InsertionSortSliced(arr, 0, arr.GetSize(), less);
}

//-----------------------------------------------------------------------------------
// ***** InsertionSort
//
// Sort an array Array, ArrayPaged, ArrayUnsafe.
// The array must have GetSize() function.
// The data type must have a defined "<" operator.
template<class Array> 
void InsertionSort(Array& arr)
{
    typedef typename Array::ValueType ValueType;
    InsertionSortSliced(arr, 0, arr.GetSize(), OperatorLess<ValueType>::Compare);
}

//-----------------------------------------------------------------------------------
// ***** Median
// Returns a median value of the input array.
// Caveats: partially sorts the array, returns a reference to the array element
// TBD: This needs to be optimized and generalized
//
template<class Array> 
typename Array::ValueType& Median(Array& arr)
{
    size_t count = arr.GetSize();
    size_t mid = (count - 1) / 2;
    OVR_ASSERT(count > 0);

	for (size_t j = 0; j <= mid; j++)
    {
		size_t min = j;
		for (size_t k = j + 1; k < count; k++)
            if (arr[k] < arr[min]) 
                min = k;
        Swap(arr[j], arr[min]);
    }
    return arr[mid];
}

//-----------------------------------------------------------------------------------
// ***** LowerBoundSliced
//
template<class Array, class Value, class Less>
size_t LowerBoundSliced(const Array& arr, size_t start, size_t end, const Value& val, Less less)
{
    intptr_t first = (intptr_t)start;
    intptr_t len   = (intptr_t)(end - start);
    intptr_t half;
    intptr_t middle;
    
    while(len > 0) 
    {
        half = len >> 1;
        middle = first + half;
        if(less(arr[middle], val)) 
        {
            first = middle + 1;
            len   = len - half - 1;
        }
        else
        {
            len = half;
        }
    }
    return (size_t)first;
}


//-----------------------------------------------------------------------------------
// ***** LowerBoundSliced
//
template<class Array, class Value>
size_t LowerBoundSliced(const Array& arr, size_t start, size_t end, const Value& val)
{
    return LowerBoundSliced(arr, start, end, val, OperatorLess<Value>::Compare);
}

//-----------------------------------------------------------------------------------
// ***** LowerBoundSized
//
template<class Array, class Value>
size_t LowerBoundSized(const Array& arr, size_t size, const Value& val)
{
    return LowerBoundSliced(arr, 0, size, val, OperatorLess<Value>::Compare);
}

//-----------------------------------------------------------------------------------
// ***** LowerBound
//
template<class Array, class Value, class Less>
size_t LowerBound(const Array& arr, const Value& val, Less less)
{
    return LowerBoundSliced(arr, 0, arr.GetSize(), val, less);
}


//-----------------------------------------------------------------------------------
// ***** LowerBound
//
template<class Array, class Value>
size_t LowerBound(const Array& arr, const Value& val)
{
    return LowerBoundSliced(arr, 0, arr.GetSize(), val, OperatorLess<Value>::Compare);
}



//-----------------------------------------------------------------------------------
// ***** UpperBoundSliced
//
template<class Array, class Value, class Less>
size_t UpperBoundSliced(const Array& arr, size_t start, size_t end, const Value& val, Less less)
{
    intptr_t first = (intptr_t)start;
    intptr_t len   = (intptr_t)(end - start);
    intptr_t half;
    intptr_t middle;
    
    while(len > 0) 
    {
        half = len >> 1;
        middle = first + half;
        if(less(val, arr[middle]))
        {
            len = half;
        }
        else 
        {
            first = middle + 1;
            len   = len - half - 1;
        }
    }
    return (size_t)first;
}


//-----------------------------------------------------------------------------------
// ***** UpperBoundSliced
//
template<class Array, class Value>
size_t UpperBoundSliced(const Array& arr, size_t start, size_t end, const Value& val)
{
    return UpperBoundSliced(arr, start, end, val, OperatorLess<Value>::Compare);
}


//-----------------------------------------------------------------------------------
// ***** UpperBoundSized
//
template<class Array, class Value>
size_t UpperBoundSized(const Array& arr, size_t size, const Value& val)
{
    return UpperBoundSliced(arr, 0, size, val, OperatorLess<Value>::Compare);
}


//-----------------------------------------------------------------------------------
// ***** UpperBound
//
template<class Array, class Value, class Less>
size_t UpperBound(const Array& arr, const Value& val, Less less)
{
    return UpperBoundSliced(arr, 0, arr.GetSize(), val, less);
}


//-----------------------------------------------------------------------------------
// ***** UpperBound
//
template<class Array, class Value>
size_t UpperBound(const Array& arr, const Value& val)
{
    return UpperBoundSliced(arr, 0, arr.GetSize(), val, OperatorLess<Value>::Compare);
}


//-----------------------------------------------------------------------------------
// ***** ReverseArray
//
template<class Array> void ReverseArray(Array& arr)
{
    intptr_t from = 0;
    intptr_t to   = arr.GetSize() - 1;
    while(from < to)
    {
        Swap(arr[from], arr[to]);
        ++from;
        --to;
    }
}


// ***** AppendArray
//
template<class CDst, class CSrc> 
void AppendArray(CDst& dst, const CSrc& src)
{
    size_t i;
    for(i = 0; i < src.GetSize(); i++) 
        dst.PushBack(src[i]);
}

//-----------------------------------------------------------------------------------
// ***** ArrayAdaptor
//
// A simple adapter that provides the GetSize() method and overloads 
// operator []. Used to wrap plain arrays in QuickSort and such.
template<class T> class ArrayAdaptor
{
public:
    typedef T ValueType;
    ArrayAdaptor() : Data(0), Size(0) {}
    ArrayAdaptor(T* ptr, size_t size) : Data(ptr), Size(size) {}
    size_t GetSize() const { return Size; }
	int GetSizeI() const { return (int)GetSize(); }
    const T& operator [] (size_t i) const { return Data[i]; }
          T& operator [] (size_t i)       { return Data[i]; }
private:
    T*      Data;
    size_t  Size;
};


//-----------------------------------------------------------------------------------
// ***** GConstArrayAdaptor
//
// A simple const adapter that provides the GetSize() method and overloads 
// operator []. Used to wrap plain arrays in LowerBound and such.
template<class T> class ConstArrayAdaptor
{
public:
    typedef T ValueType;
    ConstArrayAdaptor() : Data(0), Size(0) {}
    ConstArrayAdaptor(const T* ptr, size_t size) : Data(ptr), Size(size) {}
    size_t GetSize() const { return Size; }
	int GetSizeI() const { return (int)GetSize(); }
	const T& operator [] (size_t i) const { return Data[i]; }
private:
    const T* Data;
    size_t   Size;
};



//-----------------------------------------------------------------------------------
extern const uint8_t UpperBitTable[256];
extern const uint8_t LowerBitTable[256];



//-----------------------------------------------------------------------------------
inline uint8_t UpperBit(size_t val)
{
#ifndef OVR_64BIT_POINTERS

    if (val & 0xFFFF0000)
    {
        return (val & 0xFF000000) ? 
            UpperBitTable[(val >> 24)       ] + 24: 
            UpperBitTable[(val >> 16) & 0xFF] + 16;
    }
    return (val & 0xFF00) ?
        UpperBitTable[(val >> 8) & 0xFF] + 8:
        UpperBitTable[(val     ) & 0xFF];

#else

    if (val & 0xFFFFFFFF00000000)
    {
        if (val & 0xFFFF000000000000)
        {
            return (val & 0xFF00000000000000) ?
                UpperBitTable[(val >> 56)       ] + 56: 
                UpperBitTable[(val >> 48) & 0xFF] + 48;
        }
        return (val & 0xFF0000000000) ?
            UpperBitTable[(val >> 40) & 0xFF] + 40:
            UpperBitTable[(val >> 32) & 0xFF] + 32;
    }
    else
    {
        if (val & 0xFFFF0000)
        {
            return (val & 0xFF000000) ? 
                UpperBitTable[(val >> 24)       ] + 24: 
                UpperBitTable[(val >> 16) & 0xFF] + 16;
        }
        return (val & 0xFF00) ?
            UpperBitTable[(val >> 8) & 0xFF] + 8:
            UpperBitTable[(val     ) & 0xFF];
    }

#endif
}

//-----------------------------------------------------------------------------------
inline uint8_t LowerBit(size_t val)
{
#ifndef OVR_64BIT_POINTERS

    if (val & 0xFFFF)
    {
        return (val & 0xFF) ?
            LowerBitTable[ val & 0xFF]:
            LowerBitTable[(val >> 8) & 0xFF] + 8;
    }
    return (val & 0xFF0000) ?
            LowerBitTable[(val >> 16) & 0xFF] + 16:
            LowerBitTable[(val >> 24) & 0xFF] + 24;

#else

    if (val & 0xFFFFFFFF)
    {
        if (val & 0xFFFF)
        {
            return (val & 0xFF) ?
                LowerBitTable[ val & 0xFF]:
                LowerBitTable[(val >> 8) & 0xFF] + 8;
        }
        return (val & 0xFF0000) ?
                LowerBitTable[(val >> 16) & 0xFF] + 16:
                LowerBitTable[(val >> 24) & 0xFF] + 24;
    }
    else
    {
        if (val & 0xFFFF00000000)
        {
             return (val & 0xFF00000000) ?
                LowerBitTable[(val >> 32) & 0xFF] + 32:
                LowerBitTable[(val >> 40) & 0xFF] + 40;
        }
        return (val & 0xFF000000000000) ?
            LowerBitTable[(val >> 48) & 0xFF] + 48:
            LowerBitTable[(val >> 56) & 0xFF] + 56;
    }

#endif
}



// ******* Special (optimized) memory routines
// Note: null (bad) pointer is not tested
class MemUtil
{
public:
                                    
    // Memory compare
    static int      Cmp  (const void* p1, const void* p2, size_t byteCount)      { return memcmp(p1, p2, byteCount); }
    static int      Cmp16(const void* p1, const void* p2, size_t int16Count);
    static int      Cmp32(const void* p1, const void* p2, size_t int32Count);
    static int      Cmp64(const void* p1, const void* p2, size_t int64Count); 
};

// ** Inline Implementation

inline int MemUtil::Cmp16(const void* p1, const void* p2, size_t int16Count)
{
    int16_t*  pa  = (int16_t*)p1; 
    int16_t*  pb  = (int16_t*)p2;
    unsigned ic  = 0;
    if (int16Count == 0)
        return 0;
    while (pa[ic] == pb[ic])
        if (++ic==int16Count)
            return 0;
    return pa[ic] > pb[ic] ? 1 : -1;
}
inline int MemUtil::Cmp32(const void* p1, const void* p2, size_t int32Count)
{
    int32_t*  pa  = (int32_t*)p1;
    int32_t*  pb  = (int32_t*)p2;
    unsigned ic  = 0;
    if (int32Count == 0)
        return 0;
    while (pa[ic] == pb[ic])
        if (++ic==int32Count)
            return 0;
    return pa[ic] > pb[ic] ? 1 : -1;
}
inline int MemUtil::Cmp64(const void* p1, const void* p2, size_t int64Count)
{
    int64_t*  pa  = (int64_t*)p1;
    int64_t*  pb  = (int64_t*)p2;
    unsigned ic  = 0;
    if (int64Count == 0)
        return 0;
    while (pa[ic] == pb[ic])
        if (++ic==int64Count)
            return 0;
    return pa[ic] > pb[ic] ? 1 : -1;
}

// ** End Inline Implementation


//-----------------------------------------------------------------------------------
// ******* Byte Order Conversions
namespace ByteUtil {

    // *** Swap Byte Order

    // Swap the byte order of a byte array
    inline void     SwapOrder(void* pv, int size)
    {
        uint8_t*  pb = (uint8_t*)pv;
        uint8_t temp;
        for (int i = 0; i < size>>1; i++)
        { 
            temp            = pb[size-1-i];
            pb[size-1-i]    = pb[i];
            pb[i]           = temp; 
        }
    }

    // Swap the byte order of primitive types
    inline uint8_t  SwapOrder(uint8_t v)    { return v; }
    inline int8_t   SwapOrder(int8_t v)     { return v; }
    inline uint16_t SwapOrder(uint16_t v)   { return uint16_t(v>>8)|uint16_t(v<<8); }
    inline int16_t  SwapOrder(int16_t v)    { return int16_t((uint16_t(v)>>8)|(v<<8)); }
    inline uint32_t SwapOrder(uint32_t v)   { return (v>>24)|((v&0x00FF0000)>>8)|((v&0x0000FF00)<<8)|(v<<24); }
    inline int32_t  SwapOrder(int32_t p)    { return (int32_t)SwapOrder(uint32_t(p)); }
    inline uint64_t SwapOrder(uint64_t v)
    { 
        return   (v>>56) |
                 ((v&uint64_t(0x00FF000000000000ULL))>>40) |
                 ((v&uint64_t(0x0000FF0000000000ULL))>>24) |
                 ((v&uint64_t(0x000000FF00000000ULL))>>8)  |
                 ((v&uint64_t(0x00000000FF000000ULL))<<8)  |
                 ((v&uint64_t(0x0000000000FF0000ULL))<<24) |
                 ((v&uint64_t(0x000000000000FF00ULL))<<40) |
                 (v<<56); 
    }
    inline int64_t  SwapOrder(int64_t v)     { return (int64_t)SwapOrder(uint64_t(v)); }
    inline float    SwapOrder(float p)      
    { 
        union {
            float p;
            uint32_t v;
        } u;
        u.p = p;
        u.v = SwapOrder(u.v);
        return u.p;
    }

    inline double   SwapOrder(double p)
    { 
        union {
            double p;
            uint64_t v;
        } u;
        u.p = p;
        u.v = SwapOrder(u.v);
        return u.p;
    }
    
    // *** Byte-order conversion

#if (OVR_BYTE_ORDER == OVR_LITTLE_ENDIAN)
    // Little Endian to System (LE)
    inline uint8_t  LEToSystem(uint8_t v)   { return v; }
    inline int8_t   LEToSystem(int8_t v)    { return v; }
    inline uint16_t LEToSystem(uint16_t v)  { return v; }
    inline int16_t  LEToSystem(int16_t v)   { return v; }
    inline uint32_t LEToSystem(uint32_t v)  { return v; }
    inline int32_t  LEToSystem(int32_t v)   { return v; }
    inline uint64_t LEToSystem(uint64_t v)  { return v; }
    inline int64_t  LEToSystem(int64_t v)    { return v; }
    inline float    LEToSystem(float  v)    { return v; }
    inline double   LEToSystem(double v)    { return v; }

    // Big Endian to System (LE)
    inline uint8_t  BEToSystem(uint8_t v)   { return SwapOrder(v); }
    inline int8_t   BEToSystem(int8_t v)    { return SwapOrder(v); }
    inline uint16_t BEToSystem(uint16_t v)  { return SwapOrder(v); }
    inline int16_t  BEToSystem(int16_t v)   { return SwapOrder(v); }
    inline uint32_t BEToSystem(uint32_t v)  { return SwapOrder(v); }
    inline int32_t  BEToSystem(int32_t v)   { return SwapOrder(v); }
    inline uint64_t BEToSystem(uint64_t v)  { return SwapOrder(v); }
    inline int64_t  BEToSystem(int64_t v)    { return SwapOrder(v); }
    inline float    BEToSystem(float  v)    { return SwapOrder(v); }
    inline double   BEToSystem(double v)    { return SwapOrder(v); }

    // System (LE) to Little Endian
    inline uint8_t  SystemToLE(uint8_t v)   { return v; }
    inline int8_t   SystemToLE(int8_t v)    { return v; }
    inline uint16_t SystemToLE(uint16_t v)  { return v; }
    inline int16_t  SystemToLE(int16_t v)   { return v; }
    inline uint32_t SystemToLE(uint32_t v)  { return v; }
    inline int32_t  SystemToLE(int32_t v)   { return v; }
    inline uint64_t SystemToLE(uint64_t v)  { return v; }
    inline int64_t  SystemToLE(int64_t v)    { return v; }
    inline float    SystemToLE(float  v)    { return v; }
    inline double   SystemToLE(double v)    { return v; }   

    // System (LE) to Big Endian
    inline uint8_t  SystemToBE(uint8_t v)   { return SwapOrder(v); }
    inline int8_t   SystemToBE(int8_t v)    { return SwapOrder(v); }
    inline uint16_t SystemToBE(uint16_t v)  { return SwapOrder(v); }
    inline int16_t  SystemToBE(int16_t v)   { return SwapOrder(v); }
    inline uint32_t SystemToBE(uint32_t v)  { return SwapOrder(v); }
    inline int32_t  SystemToBE(int32_t v)   { return SwapOrder(v); }
    inline uint64_t SystemToBE(uint64_t v)  { return SwapOrder(v); }
    inline int64_t  SystemToBE(int64_t v)    { return SwapOrder(v); }
    inline float    SystemToBE(float  v)    { return SwapOrder(v); }
    inline double   SystemToBE(double v)    { return SwapOrder(v); }

#elif (OVR_BYTE_ORDER == OVR_BIG_ENDIAN)
    // Little Endian to System (BE)
    inline uint8_t  LEToSystem(uint8_t  v)  { return SwapOrder(v); }
    inline int8_t   LEToSystem(int8_t v)    { return SwapOrder(v); }
    inline uint16_t LEToSystem(uint16_t v)  { return SwapOrder(v); }
    inline int16_t  LEToSystem(int16_t v)   { return SwapOrder(v); }
    inline uint32_t LEToSystem(uint32_t v)  { return SwapOrder(v); }
    inline int32_t  LEToSystem(int32_t v)   { return SwapOrder(v); }
    inline uint64_t LEToSystem(uint64_t v)  { return SwapOrder(v); }
    inline int64_t  LEToSystem(int64_t v)    { return SwapOrder(v); }
    inline float    LEToSystem(float  v)    { return SwapOrder(v); }
    inline double   LEToSystem(double v)    { return SwapOrder(v); }

    // Big Endian to System (BE)
    inline uint8_t  BEToSystem(uint8_t v)   { return v; }
    inline int8_t   BEToSystem(int8_t v)    { return v; }
    inline uint16_t BEToSystem(uint16_t v)  { return v; }
    inline int16_t  BEToSystem(int16_t v)   { return v; }
    inline uint32_t BEToSystem(uint32_t v)  { return v; }
    inline int32_t  BEToSystem(int32_t v)   { return v; }
    inline uint64_t BEToSystem(uint64_t v)  { return v; }
    inline int64_t  BEToSystem(int64_t v)    { return v; }
    inline float    BEToSystem(float  v)    { return v; }
    inline double   BEToSystem(double v)    { return v; }

    // System (BE) to Little Endian
    inline uint8_t  SystemToLE(uint8_t v)   { return SwapOrder(v); }
    inline int8_t   SystemToLE(int8_t v)    { return SwapOrder(v); }
    inline uint16_t SystemToLE(uint16_t v)  { return SwapOrder(v); }
    inline int16_t  SystemToLE(int16_t v)   { return SwapOrder(v); }
    inline uint32_t SystemToLE(uint32_t v)  { return SwapOrder(v); }
    inline int32_t  SystemToLE(int32_t v)   { return SwapOrder(v); }
    inline uint64_t SystemToLE(uint64_t v)  { return SwapOrder(v); }
    inline int64_t  SystemToLE(int64_t v)    { return SwapOrder(v); }
    inline float    SystemToLE(float  v)    { return SwapOrder(v); }
    inline double   SystemToLE(double v)    { return SwapOrder(v); }

    // System (BE) to Big Endian
    inline uint8_t  SystemToBE(uint8_t v)   { return v; }
    inline int8_t   SystemToBE(int8_t v)    { return v; }
    inline uint16_t SystemToBE(uint16_t v)  { return v; }
    inline int16_t  SystemToBE(int16_t v)   { return v; }
    inline uint32_t SystemToBE(uint32_t v)  { return v; }
    inline int32_t  SystemToBE(int32_t v)   { return v; }
    inline uint64_t SystemToBE(uint64_t v)  { return v; }
    inline int64_t  SystemToBE(int64_t v)    { return v; }
    inline float    SystemToBE(float  v)    { return v; }
    inline double   SystemToBE(double v)    { return v; }

#else
    #error "OVR_BYTE_ORDER must be defined to OVR_LITTLE_ENDIAN or OVR_BIG_ENDIAN"
#endif

} // namespace ByteUtil



// Used primarily for hardware interfacing such as sensor reports, firmware, etc.
// Reported data is all little-endian.
inline uint16_t DecodeUInt16(const uint8_t* buffer)
{
    return ByteUtil::LEToSystem ( *(const uint16_t*)buffer );
}

inline int16_t DecodeSInt16(const uint8_t* buffer)
{
    return ByteUtil::LEToSystem ( *(const int16_t*)buffer );
}

inline uint32_t DecodeUInt32(const uint8_t* buffer)
{    
    return ByteUtil::LEToSystem ( *(const uint32_t*)buffer );
}

inline int32_t DecodeSInt32(const uint8_t* buffer)
{    
    return ByteUtil::LEToSystem ( *(const int32_t*)buffer );
}

inline float DecodeFloat(const uint8_t* buffer)
{
    union {
        uint32_t U;
        float  F;
    };

    U = DecodeUInt32(buffer);
    return F;
}

inline void EncodeUInt16(uint8_t* buffer, uint16_t val)
{
    *(uint16_t*)buffer = ByteUtil::SystemToLE ( val );
}

inline void EncodeSInt16(uint8_t* buffer, int16_t val)
{
    *(int16_t*)buffer = ByteUtil::SystemToLE ( val );
}

inline void EncodeUInt32(uint8_t* buffer, uint32_t val)
{
    *(uint32_t*)buffer = ByteUtil::SystemToLE ( val );
}

inline void EncodeSInt32(uint8_t* buffer, int32_t val)
{
    *(int32_t*)buffer = ByteUtil::SystemToLE ( val );
}

inline void EncodeFloat(uint8_t* buffer, float val)
{
    union {
        uint32_t U;
        float  F;
    };

    F = val;
    EncodeUInt32(buffer, U);
}

// Converts an 8-bit binary-coded decimal
inline int8_t DecodeBCD(uint8_t byte)
{
    uint8_t digit1 = (byte >> 4) & 0x0f;
    uint8_t digit2 = byte & 0x0f;
    int decimal = digit1 * 10 + digit2;   // maximum value = 99
    return (int8_t)decimal;
}


}} // OVR::Alg

#endif
