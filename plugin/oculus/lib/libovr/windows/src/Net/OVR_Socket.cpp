/************************************************************************************

Filename    :   OVR_Socket.cpp
Content     :   Socket common data shared between all platforms.
Created     :   June 10, 2014
Authors     :   Kevin Jenkins, Chris Taylor

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
    Address(),
    blockingTimeout(0x7fffffff)
{
}

//-----------------------------------------------------------------------------
// BerkleySocket

BerkleySocket::BerkleySocket() :
	TheSocket(INVALID_SOCKET)
  //TimeoutUsec(0) // Initialized by SetBlockingTimeout
  //TimeoutSec(0)  // "
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
  : IsListenSocket(false)
{
	Transport = TransportType_TCP;
}

TCPSocketBase::TCPSocketBase(SocketHandle handle)
  : IsListenSocket(false)
{
	TheSocket = handle;
}


}} // OVR::Net
