/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_Win32_Socket.h
Content     :   Windows-specific socket-based networking implementation
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

#ifndef OVR_Win32_Socket_h
#define OVR_Win32_Socket_h

#include "OVR_Socket.h"
#include "OVR_BitStream.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>

namespace OVR { namespace Net { 


//-----------------------------------------------------------------------------
// SockAddr

// Abstraction for IPV6 socket address, with various convenience functions
class SockAddr
{
public:
	SockAddr();
	SockAddr(SockAddr* sa);
	SockAddr(sockaddr_storage* sa);
	SockAddr(sockaddr_in6* sa);
	SockAddr(const char* hostAddress, uint16_t port, int sockType);

public:
	void   Set(const sockaddr_storage* sa);
	void   Set(const sockaddr_in6* sa);
	void   Set(const char* hostAddress, uint16_t port, int sockType); // SOCK_DGRAM or SOCK_STREAM

	uint16_t GetPort();

	String ToString(bool writePort, char portDelineator) const;
    bool IsLocalhost() const;

	void   Serialize(BitStream* bs);
	bool   Deserialize(BitStream);

	bool   operator==( const SockAddr& right ) const;
	bool   operator!=( const SockAddr& right ) const;
	bool   operator >( const SockAddr& right ) const;
	bool   operator <( const SockAddr& right ) const;

public:
	sockaddr_in6 Addr6;
};


//-----------------------------------------------------------------------------
// UDP Socket

// Windows version of TCP socket
class UDPSocket : public UDPSocketBase
{
public:
	UDPSocket();
	virtual ~UDPSocket();

public:
	virtual SocketHandle Bind(BerkleyBindParameters* pBindParameters);
	virtual int          Send(const void* pData, int bytes, SockAddr* address);
	virtual void         Poll(SocketEvent_UDP* eventHandler);

protected:
	static const int RecvBufSize = 1048576;
	uint8_t* RecvBuf;

	virtual void         OnRecv(SocketEvent_UDP* eventHandler, uint8_t* pData,
								int bytesRead, SockAddr* address);
};


//-----------------------------------------------------------------------------
// TCP Socket

// Windows version of TCP socket
class TCPSocket : public TCPSocketBase
{
    friend class TCPSocketPollState;

public:
	TCPSocket();
	TCPSocket(SocketHandle boundHandle, bool isListenSocket);
	virtual ~TCPSocket();

public:
	virtual SocketHandle Bind(BerkleyBindParameters* pBindParameters);
	virtual int          Listen();
	virtual int          Connect(SockAddr* address);
	virtual int          Send(const void* pData, int bytes);

protected:
	virtual void         OnRecv(SocketEvent_TCP* eventHandler, uint8_t* pData,
								int bytesRead);

public:
	bool IsConnecting; // Is in the process of connecting?
};


//-----------------------------------------------------------------------------
// TCPSocketPollState

// Polls multiple blocking TCP sockets at once
class TCPSocketPollState
{
    fd_set readFD, exceptionFD, writeFD;
    SocketHandle largestDescriptor;

public:
    TCPSocketPollState();
    bool IsValid() const;
    void Add(TCPSocket* tcpSocket);
    bool Poll(long usec = 30000, long seconds = 0);
    void HandleEvent(TCPSocket* tcpSocket, SocketEvent_TCP* eventHandler);
};


}} // OVR::Net

#endif
