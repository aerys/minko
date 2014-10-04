/************************************************************************************

Filename    :   OVR_Socket.cpp
Content     :   Socket common data shared between all platforms.
Created     :   June 10, 2014
Authors     :   Kevin Jenkins, Chris Taylor

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

#include "OVR_Socket.h"

namespace OVR { namespace Net {


//-----------------------------------------------------------------------------
// Socket

Socket::Socket() :
	Transport(TransportType_None)
{
}


//-----------------------------------------------------------------------------
// BerkleyBindParameters

BerkleyBindParameters::BerkleyBindParameters() :
	Port(0),
    blockingTimeout(0x7fffffff)
{
}

//-----------------------------------------------------------------------------
// BerkleySocket

BerkleySocket::BerkleySocket() :
	TheSocket(INVALID_SOCKET)
{
    SetBlockingTimeout(1000);
}

BerkleySocket::~BerkleySocket()
{
	// Close socket on destruction
	Close();
}


//-----------------------------------------------------------------------------
// UDPSocketBase

UDPSocketBase::UDPSocketBase()
{
	Transport = TransportType_UDP;
}


//-----------------------------------------------------------------------------
// TCPSocketBase

TCPSocketBase::TCPSocketBase()
{
	Transport = TransportType_TCP;
}

TCPSocketBase::TCPSocketBase(SocketHandle handle)
{
	TheSocket = handle;
}


}} // OVR::Net
