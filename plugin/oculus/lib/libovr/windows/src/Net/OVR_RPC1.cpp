/************************************************************************************

Filename    :   OVR_RPC1.cpp
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

#include "OVR_RPC1.h"
#include "OVR_BitStream.h"
#include "../Kernel/OVR_Threads.h" // Thread::MSleep
#include "OVR_MessageIDTypes.h"

namespace OVR { namespace Net { namespace Plugins {


//-----------------------------------------------------------------------------
// Types

enum {
	ID_RPC4_SIGNAL,
	CALL_BLOCKING,
	RPC_ERROR_FUNCTION_NOT_REGISTERED,
	ID_RPC4_RETURN,
};


//-----------------------------------------------------------------------------
// RPC1

RPC1::RPC1()
{
	blockingOnThisConnection = 0;
	blockingReturnValue = new BitStream();
}

RPC1::~RPC1()
{
	slotHash.Clear();
	delete blockingReturnValue;
}

void RPC1::RegisterSlot(OVR::String sharedIdentifier,  OVR::Observer<RPCSlot>* rpcSlotObserver )
{
	slotHash.AddObserverToSubject(sharedIdentifier, rpcSlotObserver);
}

bool RPC1::RegisterBlockingFunction(OVR::String uniqueID, RPCDelegate blockingFunction)
{
	if (registeredBlockingFunctions.Get(uniqueID))
		return false;

	registeredBlockingFunctions.Set(uniqueID, blockingFunction);
	return true;
}

void RPC1::UnregisterBlockingFunction(OVR::String uniqueID)
{
	registeredBlockingFunctions.Remove(uniqueID);
}

bool RPC1::CallBlocking( OVR::String uniqueID, OVR::Net::BitStream* bitStream, Ptr<Connection> pConnection, OVR::Net::BitStream* returnData )
{
    // If invalid parameters,
    if (!pConnection)
    {
        // Note: This may happen if the endpoint disconnects just before the call
        return false;
    }

	OVR::Net::BitStream out;
	out.Write((MessageID) OVRID_RPC1);
	out.Write((MessageID) CALL_BLOCKING);
	out.Write(uniqueID);
	if (bitStream)
	{
		bitStream->ResetReadPointer();
		out.AlignWriteToByteBoundary();
		out.Write(bitStream);
	}

	SendParameters sp(pConnection, out.GetData(), out.GetNumberOfBytesUsed());

    if (returnData)
    {
        returnData->Reset();
    }

    // Only one thread call at a time
    Lock::Locker singleRPCLocker(&singleRPCLock);

    // Note this does not prevent multiple calls at a time because .Wait will unlock it below.
    // The purpose of this mutex is to synchronize the polling thread and this one, not prevent
    // multiple threads from invoking RPC.
    Mutex::Locker locker(&callBlockingMutex);

    blockingReturnValue->Reset();
    blockingOnThisConnection = pConnection;

    int bytesSent = pSession->Send(&sp);
    if (bytesSent == sp.Bytes)
    {
        while (blockingOnThisConnection == pConnection)
        {
            callBlockingWait.Wait(&callBlockingMutex);
        }
    }
	else
	{
		return false;
	}

    if (returnData)
    {
        returnData->Write(blockingReturnValue);
        returnData->ResetReadPointer();
    }

	return true;
}

bool RPC1::Signal(OVR::String sharedIdentifier, OVR::Net::BitStream* bitStream, Ptr<Connection> pConnection)
{
	OVR::Net::BitStream out;
	out.Write((MessageID) OVRID_RPC1);
	out.Write((MessageID) ID_RPC4_SIGNAL);
	//out.Write(PluginId);
	out.Write(sharedIdentifier);
	if (bitStream)
	{
		bitStream->ResetReadPointer();
		out.AlignWriteToByteBoundary();
		out.Write(bitStream);
	}
	SendParameters sp(pConnection, out.GetData(), out.GetNumberOfBytesUsed());
	int32_t bytesSent = pSession->Send(&sp);
	return bytesSent == sp.Bytes;
}
void RPC1::BroadcastSignal(OVR::String sharedIdentifier, OVR::Net::BitStream* bitStream)
{
    OVR::Net::BitStream out;
    out.Write((MessageID) OVRID_RPC1);
    out.Write((MessageID) ID_RPC4_SIGNAL);
    //out.Write(PluginId);
    out.Write(sharedIdentifier);
    if (bitStream)
    {
        bitStream->ResetReadPointer();
        out.AlignWriteToByteBoundary();
        out.Write(bitStream);
    }
    BroadcastParameters p(out.GetData(), out.GetNumberOfBytesUsed());
    pSession->Broadcast(&p);
}
void RPC1::OnReceive(ReceivePayload *pPayload, ListenerReceiveResult *lrrOut)
{
	OVR_UNUSED(lrrOut);

    if (pPayload->pData[0] == OVRID_RPC1)
    {
		OVR_ASSERT(pPayload->Bytes >= 2);

		OVR::Net::BitStream bsIn((char*)pPayload->pData, pPayload->Bytes, false);
		bsIn.IgnoreBytes(2);

        if (pPayload->pData[1] == RPC_ERROR_FUNCTION_NOT_REGISTERED)
        {
            Mutex::Locker locker(&callBlockingMutex);

            blockingReturnValue->Reset();
            blockingOnThisConnection = 0;
            callBlockingWait.NotifyAll();
        }
        else if (pPayload->pData[1] == ID_RPC4_RETURN)
        {
            Mutex::Locker locker(&callBlockingMutex);

            blockingReturnValue->Reset();
			blockingReturnValue->Write(bsIn);
            blockingOnThisConnection = 0;
            callBlockingWait.NotifyAll();
		}
        else if (pPayload->pData[1] == CALL_BLOCKING)
        {
			OVR::String uniqueId;
			bsIn.Read(uniqueId);

			RPCDelegate *bf = registeredBlockingFunctions.Get(uniqueId);
			if (bf==0)
			{
				OVR::Net::BitStream bsOut;
				bsOut.Write((unsigned char) OVRID_RPC1);
				bsOut.Write((unsigned char) RPC_ERROR_FUNCTION_NOT_REGISTERED);

				SendParameters sp(pPayload->pConnection, bsOut.GetData(), bsOut.GetNumberOfBytesUsed());
				pSession->Send(&sp);

				return;
			}

			OVR::Net::BitStream returnData;
			bsIn.AlignReadToByteBoundary();
			(*bf)(&bsIn, &returnData, pPayload);

			OVR::Net::BitStream out;
			out.Write((MessageID) OVRID_RPC1);
			out.Write((MessageID) ID_RPC4_RETURN);
			returnData.ResetReadPointer();
			out.AlignWriteToByteBoundary();
			out.Write(returnData);

			SendParameters sp(pPayload->pConnection, out.GetData(), out.GetNumberOfBytesUsed());
			pSession->Send(&sp);
		}
		else if (pPayload->pData[1]==ID_RPC4_SIGNAL)
		{
			OVR::String sharedIdentifier;
			bsIn.Read(sharedIdentifier);

			Observer<RPCSlot> *o = slotHash.GetSubject(sharedIdentifier);

			if (o)
			{
				bsIn.AlignReadToByteBoundary();

				if (o)
				{
					OVR::Net::BitStream serializedParameters(bsIn.GetData() + bsIn.GetReadOffset()/8, bsIn.GetNumberOfUnreadBits()/8, false);

					o->Call(&serializedParameters, pPayload);
				}
			}
		}
	}
}

void RPC1::OnDisconnected(Connection* conn)
{
    if (blockingOnThisConnection == conn)
    {
        blockingOnThisConnection = 0;
        callBlockingWait.NotifyAll();
    }
}

void RPC1::OnConnected(Connection* conn)
{
    OVR_UNUSED(conn);
}


}}} // OVR::Net::Plugins
