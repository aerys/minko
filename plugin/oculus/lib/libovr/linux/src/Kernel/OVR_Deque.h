/************************************************************************************

Filename    :   OVR_Deque.h
Content     :   Deque container
Created     :   Nov. 15, 2013
Authors     :   Dov Katz

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

#ifndef OVR_Deque_h
#define OVR_Deque_h

#include "OVR_ContainerAllocator.h"

namespace OVR{ 

template <class Elem, class Allocator = ContainerAllocator<Elem> >
class Deque
{
public:
    enum
    {
        DefaultCapacity = 500
    };

    Deque(int capacity = DefaultCapacity);
    virtual ~Deque(void);

    virtual void         PushBack   (const Elem &Item);    // Adds Item to the end
    virtual void         PushFront  (const Elem &Item);    // Adds Item to the beginning
    virtual Elem         PopBack    (void);                // Removes Item from the end
    virtual Elem         PopFront   (void);                // Removes Item from the beginning
    virtual const Elem&  PeekBack   (int count = 0) const; // Returns count-th Item from the end
    virtual const Elem&  PeekFront  (int count = 0) const; // Returns count-th Item from the beginning

	virtual inline size_t GetSize    (void)          const; // Returns Number of Elements
	OVR_FORCE_INLINE int GetSizeI   (void)          const
	{
		return (int)GetSize();
	}
    virtual inline size_t GetCapacity(void)          const; // Returns the maximum possible number of elements
    virtual void         Clear      (void);				   // Remove all elements
    virtual inline bool  IsEmpty    ()              const;
    virtual inline bool  IsFull     ()              const;

protected:
    Elem        *Data;          // The actual Data array
    const int   Capacity;       // Deque capacity
    int         Beginning;      // Index of the first element
    int         End;            // Index of the next after last element

    // Instead of calculating the number of elements, using this variable
    // is much more convenient.
    int         ElemCount;

private:
    OVR_NON_COPYABLE(Deque);
};

template <class Elem, class Allocator = ContainerAllocator<Elem> >
class InPlaceMutableDeque : public Deque<Elem, Allocator>
{
    typedef Deque<Elem, Allocator> BaseType;

public:
    InPlaceMutableDeque( int capacity = BaseType::DefaultCapacity ) : BaseType( capacity ) {}
	virtual ~InPlaceMutableDeque() {};

    using BaseType::PeekBack;
    using BaseType::PeekFront;
	virtual Elem& PeekBack  (int count = 0); // Returns count-th Item from the end
	virtual Elem& PeekFront (int count = 0); // Returns count-th Item from the beginning
};

// Same as Deque, but allows to write more elements than maximum capacity
// Old elements are lost as they are overwritten with the new ones
template <class Elem, class Allocator = ContainerAllocator<Elem> >
class CircularBuffer : public InPlaceMutableDeque<Elem, Allocator>
{
    typedef InPlaceMutableDeque<Elem, Allocator> BaseType;

public:
    CircularBuffer(int MaxSize = BaseType::DefaultCapacity) : BaseType(MaxSize) { };
    virtual ~CircularBuffer(){}

    // The following methods are inline as a workaround for a VS bug causing erroneous C4505 warnings
    // See: http://stackoverflow.com/questions/3051992/compiler-warning-at-c-template-base-class
    inline virtual void PushBack  (const Elem &Item);    // Adds Item to the end, overwriting the oldest element at the beginning if necessary
    inline virtual void PushFront (const Elem &Item);    // Adds Item to the beginning, overwriting the oldest element at the end if necessary
};

//----------------------------------------------------------------------------------

// Deque Constructor function
template <class Elem, class Allocator>
Deque<Elem, Allocator>::Deque(int capacity) :
Capacity( capacity ), Beginning(0), End(0), ElemCount(0)
{
    Data = (Elem*) Allocator::Alloc(Capacity * sizeof(Elem));
}

// Deque Destructor function
template <class Elem, class Allocator>
Deque<Elem, Allocator>::~Deque(void)
{
    Clear();
    Allocator::Free(Data);
}

template <class Elem, class Allocator>
void Deque<Elem, Allocator>::Clear()
{
    if (!IsEmpty())
    {
        if (Beginning < End)
        {
            // no wrap-around
            Allocator::DestructArray(Data + Beginning, End - Beginning);
        }
        else
        {
            // wrap-around
            Allocator::DestructArray(Data + Beginning, Capacity - Beginning);
            Allocator::DestructArray(Data, End);
        }
    }
    
    Beginning = 0;
    End       = 0;
    ElemCount = 0;
}

// Push functions
template <class Elem, class Allocator>
void Deque<Elem, Allocator>::PushBack(const Elem &Item)
{
    // Error Check: Make sure we aren't  
    // exceeding our maximum storage space
    OVR_ASSERT( ElemCount < Capacity );

    Allocator::Construct(Data + End, Item);
    ++End;
    ++ElemCount;

    // Check for wrap-around
    if (End >= Capacity)
        End -= Capacity;
}

template <class Elem, class Allocator>
void Deque<Elem, Allocator>::PushFront(const Elem &Item)
{
    // Error Check: Make sure we aren't  
    // exceeding our maximum storage space
    OVR_ASSERT( ElemCount < Capacity );

    --Beginning;
    // Check for wrap-around
    if (Beginning < 0)
        Beginning += Capacity;

    Allocator::Construct(Data + Beginning, Item);
    ++ElemCount;
}

// Pop functions
template <class Elem, class Allocator>
Elem Deque<Elem, Allocator>::PopFront(void)
{
    // Error Check: Make sure we aren't reading from an empty Deque
    OVR_ASSERT( ElemCount > 0 );

	Elem ReturnValue = Data[ Beginning ];
    Allocator::Destruct(Data + Beginning);

	++Beginning;
    --ElemCount;

    // Check for wrap-around
    if (Beginning >= Capacity)
        Beginning -= Capacity;

    return ReturnValue;
}

template <class Elem, class Allocator>
Elem Deque<Elem, Allocator>::PopBack(void)
{
    // Error Check: Make sure we aren't reading from an empty Deque
    OVR_ASSERT( ElemCount > 0 );

    --End;
    --ElemCount;

    // Check for wrap-around
    if (End < 0)
        End += Capacity;

    Elem ReturnValue = Data[ End ];
    Allocator::Destruct(Data + End);

    return ReturnValue;
}

// Peek functions
template <class Elem, class Allocator>
const Elem& Deque<Elem, Allocator>::PeekFront(int count) const
{
    // Error Check: Make sure we aren't reading from an empty Deque
    OVR_ASSERT( ElemCount > count );

    int idx = Beginning + count;
    if (idx >= Capacity)
        idx -= Capacity;
    return Data[ idx ];
}

template <class Elem, class Allocator>
const Elem& Deque<Elem, Allocator>::PeekBack(int count) const
{
    // Error Check: Make sure we aren't reading from an empty Deque
    OVR_ASSERT( ElemCount > count );

    int idx = End - count - 1;
    if (idx < 0)
        idx += Capacity;
    return Data[ idx ];
}

// Mutable Peek functions
template <class Elem, class Allocator>
Elem& InPlaceMutableDeque<Elem, Allocator>::PeekFront(int count)
{
    // Error Check: Make sure we aren't reading from an empty Deque
    OVR_ASSERT( BaseType::ElemCount > count );

    int idx = BaseType::Beginning + count;
    if (idx >= BaseType::Capacity)
        idx -= BaseType::Capacity;
    return BaseType::Data[ idx ];
}

template <class Elem, class Allocator>
Elem& InPlaceMutableDeque<Elem, Allocator>::PeekBack(int count)
{
    // Error Check: Make sure we aren't reading from an empty Deque
    OVR_ASSERT( BaseType::ElemCount > count );

    int idx = BaseType::End - count - 1;
    if (idx < 0)
        idx += BaseType::Capacity;
    return BaseType::Data[ idx ];
}

template <class Elem, class Allocator>
inline size_t Deque<Elem, Allocator>::GetCapacity(void) const
{
    return Capacity;
}

template <class Elem, class Allocator>
inline size_t Deque<Elem, Allocator>::GetSize(void) const
{
    return ElemCount;
}

template <class Elem, class Allocator>
inline bool Deque<Elem, Allocator>::IsEmpty(void) const
{
    return ElemCount == 0;
}

template <class Elem, class Allocator>
inline bool Deque<Elem, Allocator>::IsFull(void) const
{
    return ElemCount == Capacity;
}

// ******* CircularBuffer<Elem> *******
// Push functions
template <class Elem, class Allocator>
void CircularBuffer<Elem, Allocator>::PushBack(const Elem &Item)
{
    if (this->IsFull())
        this->PopFront();
    BaseType::PushBack(Item);
}

template <class Elem, class Allocator>
void CircularBuffer<Elem, Allocator>::PushFront(const Elem &Item)
{
    if (this->IsFull())
        this->PopBack();
    BaseType::PushFront(Item);
}

};   

#endif
