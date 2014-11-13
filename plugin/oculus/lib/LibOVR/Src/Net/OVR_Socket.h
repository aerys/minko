/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_Socket.h
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

#ifndef OVR_Socket_h
#define OVR_Socket_h

#include "../Kernel/OVR_Types.h"
#include "../Kernel/OVR_Timer.h"
#include "../Kernel/OVR_Allocator.h"
#include "../Kernel/OVR_RefCount.h"
#include "../Kernel/OVR_String.h"

// OS-specific socket headers
#if defined(OVR_OS_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
# include <unistd.h>
# include <sys/types.h>
# include <netinet/in.h>
#ifdef OVR_OS_ANDROID
#include <sys/socket.h>
#endif
#endif

namespace OVR { namespace Net {

class SockAddr;
class UDPSocket;
class TCPSocket;


//-----------------------------------------------------------------------------
// Portable numeric Socket handle
#if defined(OVR_OS_WIN32)
typedef SOCKET SocketHandle;
#else
typedef int SocketHandle;
static const SocketHandle INVALID_SOCKET = -1;
static const int SOCKET_ERROR = -1;
#endif


//-----------------------------------------------------------------------------
// Types of network transport
enum TransportType
{
	TransportType_None,          // No transport (useful placeholder for invalid states)
	TransportType_Loopback,      // Loopback transport: Class talks to itself
	TransportType_TCP,           // TCP/IPv4/v6
	TransportType_UDP,           // UDP/IPv4/v6
	TransportType_PacketizedTCP  // Packetized TCP: Message framing is automatic
};


//-----------------------------------------------------------------------------
// Abstraction for a network socket. Inheritance hierarchy
// modeled after RakNet so that future support can be added
// for Linux, Windows RT, consoles, etc.
class Socket : public RefCountBase<Socket>
{
public:
	Socket();
	virtual void Close() = 0;

public:
	TransportType Transport; // Type of transport
};


//-----------------------------------------------------------------------------
// Bind parameters for Berkley sockets
struct BerkleyBindParameters
{
public:
	BerkleyBindParameters();

public:
	uint16_t Port;     // Port
	String Address;
    uint32_t blockingTimeout;
};


//-----------------------------------------------------------------------------
// Berkley socket
class BerkleySocket : public Socket
{
public:
	BerkleySocket();
	virtual ~BerkleySocket();

	virtual void   Close();
	virtual int32_t GetSockname(SockAddr* pSockAddrOut);
	virtual void   SetBlockingTimeout(int timeoutMs) // milliseconds
	{
        TimeoutSec = timeoutMs / 1000;
        TimeoutUsec = (timeoutMs % 1000) * 1000;
	}
    int            GetBlockingTimeoutUsec() const
    {
        return TimeoutUsec;
    }
    int            GetBlockingTimeoutSec() const
    {
        return TimeoutSec;
    }
    SocketHandle   GetSocketHandle() const
    {
        return TheSocket;
    }

protected:
	SocketHandle TheSocket;           // Socket handle
    int TimeoutUsec, TimeoutSec;
};


//-----------------------------------------------------------------------------
// UDP socket events
class SocketEvent_UDP
{
public:
	virtual void UDP_OnRecv(Socket* pSocket, uint8_t* pData,
							uint32_t bytesRead, SockAddr* pSockAddr)
	{
		OVR_UNUSED4(pSocket, pData, bytesRead, pSockAddr);
	}
};


//-----------------------------------------------------------------------------
// TCP socket events
class SocketEvent_TCP
{
public:
	virtual void TCP_OnRecv     (Socket* pSocket,
                                 uint8_t* pData,
                                 int bytesRead)
	{
		OVR_UNUSED3(pSocket, pData, bytesRead);
	}
	virtual void TCP_OnClosed   (TCPSocket* pSocket)
	{
		OVR_UNUSED(pSocket);
	}
	virtual void TCP_OnAccept   (TCPSocket* pListener,
                                 SockAddr* pSockAddr,
								 SocketHandle newSock)
	{
		OVR_UNUSED3(pListener, pSockAddr, newSock);
	}
	virtual void TCP_OnConnected(TCPSocket* pSocket)
	{
		OVR_UNUSED(pSocket);
	}
};


//-----------------------------------------------------------------------------
// UDP Berkley socket

// Base class for UDP sockets, code shared between platforms
class UDPSocketBase : public BerkleySocket
{
public:
	UDPSocketBase();

public:
	virtual SocketHandle Bind(BerkleyBindParameters* pBindParameters) = 0;
	virtual int          Send(const void* pData,
                              int bytes,
                              SockAddr* pSockAddr) = 0;
	virtual void         Poll(SocketEvent_UDP* eventHandler) = 0;

protected:
	virtual void         OnRecv(SocketEvent_UDP* eventHandler,
                                uint8_t* pData,
								int bytesRead,
                                SockAddr* address) = 0;
};


//-----------------------------------------------------------------------------
// TCP Berkley socket

// Base class for TCP sockets, code shared between platforms
class TCPSocketBase : public BerkleySocket
{
public:
	TCPSocketBase();
	TCPSocketBase(SocketHandle handle);

public:
	virtual SocketHandle Bind(BerkleyBindParameters* pBindParameters) = 0;
	virtual int          Listen() = 0;
	virtual int          Connect(SockAddr* pSockAddr) = 0;
	virtual int          Send(const void* pData,
                              int bytes) = 0;
protected:
	virtual void         OnRecv(SocketEvent_TCP* eventHandler,
                                uint8_t* pData,
                                int bytesRead) = 0;

protected:
	bool IsListenSocket; // Is the socket listening (acting as a server)?
};


}} // OVR::Net

#endif
