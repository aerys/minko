/************************************************************************************

PublicHeader:   OVR_Kernel.h
Filename    :   OVR_Lockless.h
Content     :   Lock-less classes for producer/consumer communication
Created     :   November 9, 2013
Authors     :   John Carmack

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

#ifndef OVR_Lockless_h
#define OVR_Lockless_h

#include "OVR_Atomic.h"

// Define this to compile-in Lockless test logic
//#define OVR_LOCKLESS_TEST

namespace OVR {


// ***** LocklessUpdater

// For single producer cases where you only care about the most recent update, not
// necessarily getting every one that happens (vsync timing, SensorFusion updates).
//
// This is multiple consumer safe, but is currently only used with a single consumer.
//
// The SlotType can be the same as T, but should probably be a larger fixed size.
// This allows for forward compatibility when the updater is shared between processes.

// FIXME: ExchangeAdd_Sync() should be replaced with a portable read-only primitive,
// so that the lockless pose state can be read-only on remote processes and to reduce
// false sharing between processes and improve performance.

template<class T, class SlotType>
class LocklessUpdater
{
public:
	LocklessUpdater() : UpdateBegin( 0 ), UpdateEnd( 0 )
    {
        OVR_COMPILER_ASSERT(sizeof(T) <= sizeof(SlotType));
    }

	T GetState() const
	{
		// Copy the state out, then retry with the alternate slot
		// if we determine that our copy may have been partially
		// stepped on by a new update.
		T	state;
		int	begin, end, final;

		for(;;)
		{
			// We are adding 0, only using these as atomic memory barriers, so it
			// is ok to cast off the const, allowing GetState() to remain const.
            end   = UpdateEnd.Load_Acquire();
            state = Slots[ end & 1 ];
            begin = UpdateBegin.Load_Acquire();
			if ( begin == end ) {
				break;
			}

			// The producer is potentially blocked while only having partially
			// written the update, so copy out the other slot.
            state = Slots[ (begin & 1) ^ 1 ];
            final = UpdateBegin.Load_Acquire();
			if ( final == begin ) {
				break;
			}

			// The producer completed the last update and started a new one before
			// we got it copied out, so try fetching the current buffer again.
		}
		return state;
	}

	void	SetState( const T& state )
	{
        const int slot = UpdateBegin.ExchangeAdd_Sync(1) & 1;
        // Write to (slot ^ 1) because ExchangeAdd returns 'previous' value before add.
        Slots[slot ^ 1] = state;
        UpdateEnd.ExchangeAdd_Sync(1);
	}

    AtomicInt<int> UpdateBegin;
    AtomicInt<int> UpdateEnd;
    SlotType       Slots[2];
};


#ifdef OVR_LOCKLESS_TEST
void StartLocklessTest();
#endif


} // namespace OVR

#endif // OVR_Lockless_h

