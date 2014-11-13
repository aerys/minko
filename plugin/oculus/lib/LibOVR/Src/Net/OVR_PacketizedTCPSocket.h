/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_PacketizedTCPSocket.cpp
Content     :   TCP with automated message framing.
Created     :   June 10, 2014
Authors     :   Kevin Jenkins

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

#ifndef OVR_PacketizedTCPSocket_h
#define OVR_PacketizedTCPSocket_h

#include "OVR_Socket.h"
#include "../Kernel/OVR_Allocator.h"
#include "../Kernel/OVR_Atomic.h"

#ifdef OVR_OS_WIN32
#include "OVR_Win32_Socket.h"
#else
#include "OVR_Unix_Socket.h"
#endif

namespace OVR { namespace Net {


//-----------------------------------------------------------------------------
// NetworkPlugin

// Packetized TCP base socket
class PacketizedTCPSocketBase : public TCPSocket
{
public:
	PacketizedTCPSocketBase() {}
	PacketizedTCPSocketBase(SocketHandle _sock, bool isListenSocket) : TCPSocket(_sock, isListenSocket) {}
};


//-----------------------------------------------------------------------------
// PacketizedTCPSocket

// Uses TCP but is message aligned rather than stream aligned
// Alternative to reliable UDP
class PacketizedTCPSocket : public PacketizedTCPSocketBase
{
public:
	PacketizedTCPSocket();
	PacketizedTCPSocket(SocketHandle _sock, bool isListenSocket);
	virtual ~PacketizedTCPSocket();

public:
	virtual int Send(const void* pData, int bytes);
	virtual int SendAndConcatenate(const void** pDataArray, int *dataLengthArray, int arrayCount);

protected:
	virtual void OnRecv(SocketEvent_TCP* eventHandler, uint8_t* pData, int bytesRead);

	int BytesFromStream(uint8_t* pData, int bytesRead);

    Lock   sendLock;
    Lock   recvBuffLock;

	uint8_t* pRecvBuff;     // Queued receive buffered data
	int    pRecvBuffSize; // Size of receive queue in bytes
};


}} // OVR::Net

#endif
