/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_RPC1.h
Content     :   A network plugin that provides remote procedure call functionality.
Created     :   June 10, 2014
Authors     :   Kevin Jenkins

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

#ifndef OVR_Net_RPC_h
#define OVR_Net_RPC_h

#include "OVR_NetworkPlugin.h"
#include "../Kernel/OVR_Hash.h"
#include "../Kernel/OVR_String.h"
#include "OVR_BitStream.h"
#include "../Kernel/OVR_Threads.h"
#include "../Kernel/OVR_Delegates.h"
#include "../Kernel//OVR_Observer.h"

namespace OVR { namespace Net { namespace Plugins {


typedef Delegate3<void, BitStream*, BitStream*, ReceivePayload*> RPCDelegate;
typedef Delegate2<void, BitStream*, ReceivePayload*> RPCSlot;
// typedef void ( *Slot ) ( OVR::Net::BitStream *userData, OVR::Net::ReceivePayload *pPayload );

/// NetworkPlugin that maps strings to function pointers. Can invoke the functions using blocking calls with return values, or signal/slots. Networked parameters serialized with BitStream
class RPC1 : public NetworkPlugin, public NewOverrideBase
{
public:
	RPC1();
	virtual ~RPC1();

	/// Register a slot, which is a function pointer to one or more implementations that supports this function signature
	/// When a signal occurs, all slots with the same identifier are called.
	/// \param[in] sharedIdentifier A string to identify the slot. Recommended to be the same as the name of the function.
	/// \param[in] functionPtr Pointer to the function.
	/// \param[in] callPriority Slots are called by order of the highest callPriority first. For slots with the same priority, they are called in the order they are registered
	void RegisterSlot(OVR::String sharedIdentifier,  OVR::Observer<RPCSlot> *rpcSlotObserver);

	/// \brief Same as \a RegisterFunction, but is called with CallBlocking() instead of Call() and returns a value to the caller
	bool RegisterBlockingFunction(OVR::String uniqueID, RPCDelegate blockingFunction);

	/// \brief Same as UnregisterFunction, except for a blocking function
	void UnregisterBlockingFunction(OVR::String uniqueID);

	// \brief Same as call, but don't return until the remote system replies.
	/// Broadcasting parameter does not exist, this can only call one remote system
	/// \note This function does not return until the remote system responds, disconnects, or was never connected to begin with
	/// \param[in] Identifier originally passed to RegisterBlockingFunction() on the remote system(s)
	/// \param[in] bitStream bitStream encoded data to send to the function callback
	/// \param[in] pConnection connection to send on
	/// \param[out] returnData Written to by the function registered with RegisterBlockingFunction.
	/// \return true if successfully called. False on disconnect, function not registered, or not connected to begin with
	bool CallBlocking( OVR::String uniqueID, OVR::Net::BitStream * bitStream, Ptr<Connection> pConnection, OVR::Net::BitStream *returnData = NULL );

	/// Calls zero or more functions identified by sharedIdentifier registered with RegisterSlot()
	/// \param[in] sharedIdentifier parameter of the same name passed to RegisterSlot() on the remote system
	/// \param[in] bitStream bitStream encoded data to send to the function callback
	/// \param[in] pConnection connection to send on
	bool Signal(OVR::String sharedIdentifier, OVR::Net::BitStream * bitStream, Ptr<Connection> pConnection);
    void BroadcastSignal(OVR::String sharedIdentifier, OVR::Net::BitStream * bitStream);


protected:
	virtual void OnReceive(ReceivePayload *pPayload, ListenerReceiveResult *lrrOut);

    virtual void OnDisconnected(Connection* conn);
    virtual void OnConnected(Connection* conn);

	Hash< String, RPCDelegate, String::HashFunctor > registeredBlockingFunctions;
	ObserverHash< RPCSlot > slotHash;

    // Synchronization for RPC caller
    Lock            singleRPCLock;
    Mutex           callBlockingMutex;
    WaitCondition   callBlockingWait;

    Net::BitStream* blockingReturnValue;
	Ptr<Connection> blockingOnThisConnection;
};


}}} // OVR::Net::Plugins

#endif // OVR_Net_RPC_h
