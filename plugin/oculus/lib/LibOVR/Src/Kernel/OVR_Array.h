/************************************************************************************

PublicHeader:   OVR_Kernel.h
Filename    :   OVR_Array.h
Content     :   Template implementation for Array
Created     :   September 19, 2012
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

#ifndef OVR_Array_h
#define OVR_Array_h

#include "OVR_ContainerAllocator.h"

namespace OVR {

//-----------------------------------------------------------------------------------
// ***** ArrayDefaultPolicy
//
// Default resize behavior. No minimal capacity, Granularity=4, 
// Shrinking as needed. ArrayConstPolicy actually is the same as 
// ArrayDefaultPolicy, but parametrized with constants. 
// This struct is used only in order to reduce the template "matroska".
struct ArrayDefaultPolicy
{
    ArrayDefaultPolicy() : Capacity(0) {}
    ArrayDefaultPolicy(const ArrayDefaultPolicy&) : Capacity(0) {}

    size_t GetMinCapacity() const { return 0; }
    size_t GetGranularity() const { return 4; }
    bool  NeverShrinking() const { return 0; }

    size_t GetCapacity()    const      { return Capacity; }
    void  SetCapacity(size_t capacity) { Capacity = capacity; }
private:
    size_t Capacity;
};


//-----------------------------------------------------------------------------------
// ***** ArrayConstPolicy
//
// Statically parametrized resizing behavior:
// MinCapacity, Granularity, and Shrinking flag.
template<int MinCapacity=0, int Granularity=4, bool NeverShrink=false>
struct ArrayConstPolicy
{
    typedef ArrayConstPolicy<MinCapacity, Granularity, NeverShrink> SelfType;

    ArrayConstPolicy() : Capacity(0) {}
    ArrayConstPolicy(const SelfType&) : Capacity(0) {}

    size_t GetMinCapacity() const { return MinCapacity; }
    size_t GetGranularity() const { return Granularity; }
    bool  NeverShrinking() const { return NeverShrink; }

    size_t GetCapacity()    const      { return Capacity; }
    void  SetCapacity(size_t capacity) { Capacity = capacity; }
private:
    size_t Capacity;
};

//-----------------------------------------------------------------------------------
// ***** ArrayDataBase
//
// Basic operations with array data: Reserve, Resize, Free, ArrayPolicy.
// For internal use only: ArrayData,ArrayDataCC and others.
template<class T, class Allocator, class SizePolicy>
struct ArrayDataBase
{
    typedef T                                           ValueType;
    typedef Allocator                                   AllocatorType;
    typedef SizePolicy                                  SizePolicyType;
    typedef ArrayDataBase<T, Allocator, SizePolicy>     SelfType;

    ArrayDataBase()
        : Data(0), Size(0), Policy() {}

    ArrayDataBase(const SizePolicy& p)
        : Data(0), Size(0), Policy(p) {}

    ~ArrayDataBase() 
    {
        Allocator::DestructArray(Data, Size);
        Allocator::Free(Data);
    }

    size_t GetCapacity() const 
    { 
        return Policy.GetCapacity(); 
    }

    void ClearAndRelease()
    {
        Allocator::DestructArray(Data, Size);
        Allocator::Free(Data);
        Data = 0;
        Size = 0;
        Policy.SetCapacity(0);
    }

    void Reserve(size_t newCapacity)
    {
        if (Policy.NeverShrinking() && newCapacity < GetCapacity())
            return;

        if (newCapacity < Policy.GetMinCapacity())
            newCapacity = Policy.GetMinCapacity();

        // Resize the buffer.
        if (newCapacity == 0)
        {
            if (Data)
            {
                Allocator::Free(Data);
                Data = 0;
            }
            Policy.SetCapacity(0);
        }
        else
        {
            size_t gran = Policy.GetGranularity();
            newCapacity = (newCapacity + gran - 1) / gran * gran;
            if (Data)
            {
                if (Allocator::IsMovable())
                {
                    Data = (T*)Allocator::Realloc(Data, sizeof(T) * newCapacity);
                }
                else
                {
                    T* newData = (T*)Allocator::Alloc(sizeof(T) * newCapacity);
                    size_t i, s;
                    s = (Size < newCapacity) ? Size : newCapacity;
                    for (i = 0; i < s; ++i)
                    {
                        Allocator::Construct(&newData[i], Data[i]);
                        Allocator::Destruct(&Data[i]);
                    }
                    for (i = s; i < Size; ++i)
                    {
                        Allocator::Destruct(&Data[i]);
                    }
                    Allocator::Free(Data);
                    Data = newData;
                }
            }
            else
            {
                Data = (T*)Allocator::Alloc(sizeof(T) * newCapacity);
                //memset(Buffer, 0, (sizeof(ValueType) * newSize)); // Do we need this?
            }
            Policy.SetCapacity(newCapacity);
            // OVR_ASSERT(Data); // need to throw (or something) on alloc failure!
        }
    }

    // This version of Resize DOES NOT construct the elements.
    // It's done to optimize PushBack, which uses a copy constructor 
    // instead of the default constructor and assignment
    void ResizeNoConstruct(size_t newSize)
    {
        size_t oldSize = Size;

        if (newSize < oldSize)
        {
            Allocator::DestructArray(Data + newSize, oldSize - newSize);
            if (newSize < (Policy.GetCapacity() >> 1))
            {
                Reserve(newSize);
            }
        }
        else if(newSize >= Policy.GetCapacity())
        {
            Reserve(newSize + (newSize >> 2));
        }
        //! IMPORTANT to modify Size only after Reserve completes, because garbage collectable
        // array may use this array and may traverse it during Reserve (in the case, if 
        // collection occurs because of heap limit exceeded).
        Size = newSize;
    }

    ValueType*  Data;
    size_t      Size;
    SizePolicy  Policy;
};



//-----------------------------------------------------------------------------------
// ***** ArrayData
//
// General purpose array data.
// For internal use only in Array, ArrayLH, ArrayPOD and so on.
template<class T, class Allocator, class SizePolicy>
struct ArrayData : ArrayDataBase<T, Allocator, SizePolicy>
{
    typedef T ValueType;
    typedef Allocator                                   AllocatorType;
    typedef SizePolicy                                  SizePolicyType;
    typedef ArrayDataBase<T, Allocator, SizePolicy>     BaseType;
    typedef ArrayData    <T, Allocator, SizePolicy>     SelfType;

    ArrayData()
        : BaseType() { }

    ArrayData(size_t size)
        : BaseType() { Resize(size); }

    ArrayData(const SelfType& a)
        : BaseType(a.Policy) { Append(a.Data, a.Size); }


    void Resize(size_t newSize)
    {
        size_t oldSize = this->Size;
        BaseType::ResizeNoConstruct(newSize);
        if(newSize > oldSize)
            Allocator::ConstructArray(this->Data + oldSize, newSize - oldSize);
    }

    void PushBack(const ValueType& val)
    {
        BaseType::ResizeNoConstruct(this->Size + 1);
        Allocator::Construct(this->Data + this->Size - 1, val);
    }

    template<class S>
    void PushBackAlt(const S& val)
    {
        BaseType::ResizeNoConstruct(this->Size + 1);
        Allocator::ConstructAlt(this->Data + this->Size - 1, val);
    }

    // Append the given data to the array.
    void Append(const ValueType other[], size_t count)
    {
        if (count)
        {
            size_t oldSize = this->Size;
            BaseType::ResizeNoConstruct(this->Size + count);
            Allocator::ConstructArray(this->Data + oldSize, count, other);
        }
    }
};



//-----------------------------------------------------------------------------------
// ***** ArrayDataCC
//
// A modification of ArrayData that always copy-constructs new elements
// using a specified DefaultValue. For internal use only in ArrayCC.
template<class T, class Allocator, class SizePolicy>
struct ArrayDataCC : ArrayDataBase<T, Allocator, SizePolicy>
{
    typedef T                                           ValueType;
    typedef Allocator                                   AllocatorType;
    typedef SizePolicy                                  SizePolicyType;
    typedef ArrayDataBase<T, Allocator, SizePolicy>     BaseType;
    typedef ArrayDataCC  <T, Allocator, SizePolicy>     SelfType;

    ArrayDataCC(const ValueType& defval)
        : BaseType(), DefaultValue(defval) { }

    ArrayDataCC(const ValueType& defval, size_t size)
        : BaseType(), DefaultValue(defval) { Resize(size); }

    ArrayDataCC(const SelfType& a)
        : BaseType(a.Policy), DefaultValue(a.DefaultValue) { Append(a.Data, a.Size); }


    void Resize(size_t newSize)
    {
        size_t oldSize = this->Size;
        BaseType::ResizeNoConstruct(newSize);
        if(newSize > oldSize)
            Allocator::ConstructArray(this->Data + oldSize, newSize - oldSize, DefaultValue);
    }

    void PushBack(const ValueType& val)
    {
        BaseType::ResizeNoConstruct(this->Size + 1);
        Allocator::Construct(this->Data + this->Size - 1, val);
    }

    template<class S>
    void PushBackAlt(const S& val)
    {
        BaseType::ResizeNoConstruct(this->Size + 1);
        Allocator::ConstructAlt(this->Data + this->Size - 1, val);
    }

    // Append the given data to the array.
    void Append(const ValueType other[], size_t count)
    {
        if (count)
        {
            size_t oldSize = this->Size;
            BaseType::ResizeNoConstruct(this->Size + count);
            Allocator::ConstructArray(this->Data + oldSize, count, other);
        }
    }

    ValueType   DefaultValue;
};





//-----------------------------------------------------------------------------------
// ***** ArrayBase
//
// Resizable array. The behavior can be POD (suffix _POD) and 
// Movable (no suffix) depending on the allocator policy.
// In case of _POD the constructors and destructors are not called.
// 
// Arrays can't handle non-movable objects! Don't put anything in here 
// that can't be moved around by bitwise copy. 
// 
// The addresses of elements are not persistent! Don't keep the address 
// of an element; the array contents will move around as it gets resized.
template<class ArrayData>
class ArrayBase
{
public:
    typedef typename ArrayData::ValueType       ValueType;
    typedef typename ArrayData::AllocatorType   AllocatorType;
    typedef typename ArrayData::SizePolicyType  SizePolicyType;
    typedef ArrayBase<ArrayData>                SelfType;


#undef new
    OVR_MEMORY_REDEFINE_NEW(ArrayBase)
// Redefine operator 'new' if necessary.
#if defined(OVR_DEFINE_NEW)
#define new OVR_DEFINE_NEW
#endif


    ArrayBase()
        : Data() {}
    ArrayBase(size_t size)
        : Data(size) {}
    ArrayBase(const SelfType& a)
        : Data(a.Data) {}

    ArrayBase(const ValueType& defval)
        : Data(defval) {}
    ArrayBase(const ValueType& defval, size_t size)
        : Data(defval, size) {}
  
    SizePolicyType* GetSizePolicy() const                  { return Data.Policy; }
    void            SetSizePolicy(const SizePolicyType& p) { Data.Policy = p; }

    bool    NeverShrinking()const       { return Data.Policy.NeverShrinking(); }
	size_t  GetSize()       const       { return Data.Size; }
	int     GetSizeI()      const       { return (int)Data.Size; }
	bool    IsEmpty()       const       { return Data.Size == 0; }
    size_t  GetCapacity()   const       { return Data.GetCapacity(); }
    size_t  GetNumBytes()   const       { return Data.GetCapacity() * sizeof(ValueType); }

    void    ClearAndRelease()           { Data.ClearAndRelease(); }
    void    Clear()                     { Data.Resize(0); }
    void    Resize(size_t newSize)       { Data.Resize(newSize); }

    // Reserve can only increase the capacity
    void    Reserve(size_t newCapacity)  
    { 
        if (newCapacity > Data.GetCapacity())
            Data.Reserve(newCapacity); 
    }

    // Basic access.
    ValueType& At(size_t index)
    {
        OVR_ASSERT(index < Data.Size);
        return Data.Data[index]; 
    }
    const ValueType& At(size_t index) const
    {
        OVR_ASSERT(index < Data.Size);
        return Data.Data[index]; 
    }

    ValueType ValueAt(size_t index) const
    {
        OVR_ASSERT(index < Data.Size);
        return Data.Data[index]; 
    }

    // Basic access.
    ValueType& operator [] (size_t index)
    {
        OVR_ASSERT(index < Data.Size);
        return Data.Data[index]; 
    }
    const ValueType& operator [] (size_t index) const
    {
        OVR_ASSERT(index < Data.Size);
        return Data.Data[index]; 
    }

    // Raw pointer to the data. Use with caution!
    const ValueType* GetDataPtr() const { return Data.Data; }
          ValueType* GetDataPtr()       { return Data.Data; }

    // Insert the given element at the end of the array.
    void    PushBack(const ValueType& val)
    {
        // DO NOT pass elements of your own vector into
        // push_back()!  Since we're using references,
        // resize() may munge the element storage!
        // OVR_ASSERT(&val < &Buffer[0] || &val > &Buffer[BufferSize]);
        Data.PushBack(val);
    }

    template<class S>
    void PushBackAlt(const S& val)
    {
        Data.PushBackAlt(val);
    }

    // Remove the last element.
    void    PopBack(size_t count = 1)
    {
        OVR_ASSERT(Data.Size >= count);
        Data.Resize(Data.Size - count);
    }

    ValueType& PushDefault()
    {
        Data.PushBack(ValueType());
        return Back();
    }

    ValueType Pop()
    {
        ValueType t = Back();
        PopBack();
        return t;
    }


    // Access the first element.
    ValueType&          Front()         { return At(0); }
    const ValueType&    Front() const   { return At(0); }

    // Access the last element.
    ValueType&          Back()          { return At(Data.Size - 1); }
    const ValueType&    Back() const    { return At(Data.Size - 1); }

    // Array copy.  Copies the contents of a into this array.
    const SelfType& operator = (const SelfType& a)   
    {
        Resize(a.GetSize());
        for (size_t i = 0; i < Data.Size; i++) {
            *(Data.Data + i) = a[i];
        }
        return *this;
    }

    // Removing multiple elements from the array.
    void    RemoveMultipleAt(size_t index, size_t num)
    {
        OVR_ASSERT(index + num <= Data.Size);
        if (Data.Size == num)
        {
            Clear();
        }
        else
        {
            AllocatorType::DestructArray(Data.Data + index, num);
            AllocatorType::CopyArrayForward(
                Data.Data + index, 
                Data.Data + index + num,
                Data.Size - num - index);
            Data.Size -= num;
        }
    }

    // Removing an element from the array is an expensive operation!
    // It compacts only after removing the last element.
    // If order of elements in the array is not important then use 
    // RemoveAtUnordered, that could be much faster than the regular
    // RemoveAt.
    void    RemoveAt(size_t index)
    {
        OVR_ASSERT(index < Data.Size);
        if (Data.Size == 1)
        {
            Clear();
        }
        else
        {
            AllocatorType::Destruct(Data.Data + index);
            AllocatorType::CopyArrayForward(
                Data.Data + index, 
                Data.Data + index + 1,
                Data.Size - 1 - index);
            --Data.Size;
        }
    }

    // Removes an element from the array without respecting of original order of 
    // elements for better performance. Do not use on array where order of elements
    // is important, otherwise use it instead of regular RemoveAt().
    void    RemoveAtUnordered(size_t index)
    {
        OVR_ASSERT(index < Data.Size);
        if (Data.Size == 1)
        {
            Clear();
        }
        else
        {
            // copy the last element into the 'index' position 
            // and decrement the size (instead of moving all elements
            // in [index + 1 .. size - 1] range).
            const size_t lastElemIndex = Data.Size - 1;
            if (index < lastElemIndex)
            {
                AllocatorType::Destruct(Data.Data + index);
                AllocatorType::Construct(Data.Data + index, Data.Data[lastElemIndex]);
            }
            AllocatorType::Destruct(Data.Data + lastElemIndex);
            --Data.Size;
        }
    }

    // Insert the given object at the given index shifting all the elements up.
    void    InsertAt(size_t index, const ValueType& val = ValueType())
    {
        OVR_ASSERT(index <= Data.Size);

        Data.Resize(Data.Size + 1);
        if (index < Data.Size - 1)
        {
            AllocatorType::CopyArrayBackward(
                Data.Data + index + 1, 
                Data.Data + index, 
                Data.Size - 1 - index);
        }
        AllocatorType::Construct(Data.Data + index, val);
    }

    // Insert the given object at the given index shifting all the elements up.
    void    InsertMultipleAt(size_t index, size_t num, const ValueType& val = ValueType())
    {
        OVR_ASSERT(index <= Data.Size);

        Data.Resize(Data.Size + num);
        if (index < Data.Size - num)
        {
            AllocatorType::CopyArrayBackward(
                Data.Data + index + num,
                Data.Data + index,
                Data.Size - num - index);
        }
        for (size_t i = 0; i < num; ++i)
            AllocatorType::Construct(Data.Data + index + i, val);
    }

    // Append the given data to the array.
    void    Append(const SelfType& other)
    {
        Append(other.Data.Data, other.GetSize());
    }

    // Append the given data to the array.
    void    Append(const ValueType other[], size_t count)
    {
        Data.Append(other, count);
    }

    class Iterator
    {
        SelfType*       pArray;
        intptr_t        CurIndex;

    public:
        Iterator() : pArray(0), CurIndex(-1) {}
        Iterator(SelfType* parr, intptr_t idx = 0) : pArray(parr), CurIndex(idx) {}

        bool operator==(const Iterator& it) const { return pArray == it.pArray && CurIndex == it.CurIndex; }
        bool operator!=(const Iterator& it) const { return pArray != it.pArray || CurIndex != it.CurIndex; }

        Iterator& operator++()
        {
            if (pArray)
            {
                if (CurIndex < (intptr_t)pArray->GetSize())
                    ++CurIndex;
            }
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator it(*this);
            operator++();
            return it;
        }
        Iterator& operator--()
        {
            if (pArray)
            {
                if (CurIndex >= 0)
                    --CurIndex;
            }
            return *this;
        }
        Iterator operator--(int)
        {
            Iterator it(*this);
            operator--();
            return it;
        }
        Iterator operator+(int delta) const
        {
            return Iterator(pArray, CurIndex + delta);
        }
        Iterator operator-(int delta) const
        {
            return Iterator(pArray, CurIndex - delta);
        }
        intptr_t operator-(const Iterator& right) const
        {
            OVR_ASSERT(pArray == right.pArray);
            return CurIndex - right.CurIndex;
        }
        ValueType& operator*() const    { OVR_ASSERT(pArray); return  (*pArray)[CurIndex]; }
        ValueType* operator->() const   { OVR_ASSERT(pArray); return &(*pArray)[CurIndex]; }
        ValueType* GetPtr() const       { OVR_ASSERT(pArray); return &(*pArray)[CurIndex]; }

        bool IsFinished() const { return !pArray || CurIndex < 0 || CurIndex >= (int)pArray->GetSize(); }

        void Remove()
        {
            if (!IsFinished())
                pArray->RemoveAt(CurIndex);
        }

        intptr_t GetIndex() const { return CurIndex; }
    };

    Iterator Begin() { return Iterator(this); }
    Iterator End()   { return Iterator(this, (intptr_t)GetSize()); }
    Iterator Last()  { return Iterator(this, (intptr_t)GetSize() - 1); }

    class ConstIterator
    {
        const SelfType* pArray;
        intptr_t        CurIndex;

    public:
        ConstIterator() : pArray(0), CurIndex(-1) {}
        ConstIterator(const SelfType* parr, intptr_t idx = 0) : pArray(parr), CurIndex(idx) {}

        bool operator==(const ConstIterator& it) const { return pArray == it.pArray && CurIndex == it.CurIndex; }
        bool operator!=(const ConstIterator& it) const { return pArray != it.pArray || CurIndex != it.CurIndex; }

        ConstIterator& operator++()
        {
            if (pArray)
            {
                if (CurIndex < (int)pArray->GetSize())
                    ++CurIndex;
            }
            return *this;
        }
        ConstIterator operator++(int)
        {
            ConstIterator it(*this);
            operator++();
            return it;
        }
        ConstIterator& operator--()
        {
            if (pArray)
            {
                if (CurIndex >= 0)
                    --CurIndex;
            }
            return *this;
        }
        ConstIterator operator--(int)
        {
            ConstIterator it(*this);
            operator--();
            return it;
        }
        ConstIterator operator+(int delta) const
        {
            return ConstIterator(pArray, CurIndex + delta);
        }
        ConstIterator operator-(int delta) const
        {
            return ConstIterator(pArray, CurIndex - delta);
        }
        intptr_t operator-(const ConstIterator& right) const
        {
            OVR_ASSERT(pArray == right.pArray);
            return CurIndex - right.CurIndex;
        }
        const ValueType& operator*() const  { OVR_ASSERT(pArray); return  (*pArray)[CurIndex]; }
        const ValueType* operator->() const { OVR_ASSERT(pArray); return &(*pArray)[CurIndex]; }
        const ValueType* GetPtr() const     { OVR_ASSERT(pArray); return &(*pArray)[CurIndex]; }

        bool IsFinished() const { return !pArray || CurIndex < 0 || CurIndex >= (int)pArray->GetSize(); }

        intptr_t GetIndex()  const { return CurIndex; }
    };
    ConstIterator Begin() const { return ConstIterator(this); }
    ConstIterator End() const   { return ConstIterator(this, (intptr_t)GetSize()); }
    ConstIterator Last() const  { return ConstIterator(this, (intptr_t)GetSize() - 1); }

protected:
    ArrayData   Data;
};



//-----------------------------------------------------------------------------------
// ***** Array
//
// General purpose array for movable objects that require explicit 
// construction/destruction.
template<class T, class SizePolicy=ArrayDefaultPolicy>
class Array : public ArrayBase<ArrayData<T, ContainerAllocator<T>, SizePolicy> >
{
public:
    typedef T                                                           ValueType;
    typedef ContainerAllocator<T>                                       AllocatorType;
    typedef SizePolicy                                                  SizePolicyType;
    typedef Array<T, SizePolicy>                                        SelfType;
    typedef ArrayBase<ArrayData<T, ContainerAllocator<T>, SizePolicy> > BaseType;

    Array() : BaseType() {}
    Array(size_t size) : BaseType(size) {}
    Array(const SizePolicyType& p) : BaseType() { SetSizePolicy(p); }
    Array(const SelfType& a) : BaseType(a) {}
    const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }
};

// ***** ArrayPOD
//
// General purpose array for movable objects that DOES NOT require  
// construction/destruction. Constructors and destructors are not called! 
// Global heap is in use.
template<class T, class SizePolicy=ArrayDefaultPolicy>
class ArrayPOD : public ArrayBase<ArrayData<T, ContainerAllocator_POD<T>, SizePolicy> >
{
public:
    typedef T                                                               ValueType;
    typedef ContainerAllocator_POD<T>                                       AllocatorType;
    typedef SizePolicy                                                      SizePolicyType;
    typedef ArrayPOD<T, SizePolicy>                                         SelfType;
    typedef ArrayBase<ArrayData<T, ContainerAllocator_POD<T>, SizePolicy> > BaseType;

    ArrayPOD() : BaseType() {}
    ArrayPOD(size_t size) : BaseType(size) {}
    ArrayPOD(const SizePolicyType& p) : BaseType() { SetSizePolicy(p); }
    ArrayPOD(const SelfType& a) : BaseType(a) {}
    const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }
};


// ***** ArrayCPP
//
// General purpose, fully C++ compliant array. Can be used with non-movable data.
// Global heap is in use.
template<class T, class SizePolicy=ArrayDefaultPolicy>
class ArrayCPP : public ArrayBase<ArrayData<T, ContainerAllocator_CPP<T>, SizePolicy> >
{
public:
    typedef T                                                               ValueType;
    typedef ContainerAllocator_CPP<T>                                       AllocatorType;
    typedef SizePolicy                                                      SizePolicyType;
    typedef ArrayCPP<T, SizePolicy>                                         SelfType;
    typedef ArrayBase<ArrayData<T, ContainerAllocator_CPP<T>, SizePolicy> > BaseType;

    ArrayCPP() : BaseType() {}
    ArrayCPP(size_t size) : BaseType(size) {}
    ArrayCPP(const SizePolicyType& p) : BaseType() { SetSizePolicy(p); }
    ArrayCPP(const SelfType& a) : BaseType(a) {}
    const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }
};


// ***** ArrayCC
//
// A modification of the array that uses the given default value to
// construct the elements. The constructors and destructors are 
// properly called, the objects must be movable.

template<class T, class SizePolicy=ArrayDefaultPolicy>
class ArrayCC : public ArrayBase<ArrayDataCC<T, ContainerAllocator<T>, SizePolicy> >
{
public:
    typedef T                                                               ValueType;
    typedef ContainerAllocator<T>                                           AllocatorType;
    typedef SizePolicy                                                      SizePolicyType;
    typedef ArrayCC<T, SizePolicy>                                          SelfType;
    typedef ArrayBase<ArrayDataCC<T, ContainerAllocator<T>, SizePolicy> >   BaseType;

    ArrayCC(const ValueType& defval) : BaseType(defval) {}
    ArrayCC(const ValueType& defval, size_t size) : BaseType(defval, size) {}
    ArrayCC(const ValueType& defval, const SizePolicyType& p) : BaseType(defval) { SetSizePolicy(p); }
    ArrayCC(const SelfType& a) : BaseType(a) {}
    const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }
};

} // OVR

#endif
