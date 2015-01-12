/************************************************************************************

Filename    :   OVR_Unix_Socket.cpp
Content     :   Berkley sockets networking implementation
Created     :   July 1, 2014
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

#include "OVR_Unix_Socket.h"
#include "../Kernel/OVR_Std.h"
#include "../Kernel/OVR_Allocator.h"
#include "../Kernel/OVR_Threads.h" // Thread::MSleep
#include "../Kernel/OVR_Log.h"

#include <errno.h>

namespace OVR { namespace Net {

//-----------------------------------------------------------------------------
// BerkleySocket

void BerkleySocket::Close()
{
	if (TheSocket != INVALID_SOCKET)
	{
		close(TheSocket);
		TheSocket = INVALID_SOCKET;
	}
}

SInt32 BerkleySocket::GetSockname(SockAddr *pSockAddrOut)
{
	struct sockaddr_in6 sa;
	memset(&sa,0,sizeof(sa));
	socklen_t size = sizeof(sa);
	SInt32 i = getsockname(TheSocket, (sockaddr*)&sa, &size);
	if (i>=0)
	{
		pSockAddrOut->Set(&sa);
	}
	return i;
}


//-----------------------------------------------------------------------------
// BitStream overloads for SockAddr

BitStream& operator<<(BitStream& out, SockAddr& in)
{
	out.WriteBits((const unsigned char*) &in.Addr6, sizeof(in.Addr6)*8, true);
	return out;
}

BitStream& operator>>(BitStream& in, SockAddr& out)
{
	bool success = in.ReadBits((unsigned char*) &out.Addr6, sizeof(out.Addr6)*8, true);
	OVR_ASSERT(success);
	OVR_UNUSED(success);
	return in;
}


//-----------------------------------------------------------------------------
// SockAddr

SockAddr::SockAddr()
{
}

SockAddr::SockAddr(SockAddr* address)
{
	Set(&address->Addr6);
}

SockAddr::SockAddr(sockaddr_storage* storage)
{
	Set(storage);
}

SockAddr::SockAddr(sockaddr_in6* address)
{
	Set(address);
}

SockAddr::SockAddr(const char* hostAddress, UInt16 port, int sockType)
{
	Set(hostAddress, port, sockType);
}

void SockAddr::Set(const sockaddr_storage* storage)
{
	memcpy(&Addr6, storage, sizeof(Addr6));
}

void SockAddr::Set(const sockaddr_in6* address)
{
	memcpy(&Addr6, address, sizeof(Addr6));
}

void SockAddr::Set(const char* hostAddress, UInt16 port, int sockType)
{
	memset(&Addr6, 0, sizeof(Addr6));

	struct addrinfo hints;

	// make sure the struct is empty
	memset(&hints, 0, sizeof (addrinfo));

	hints.ai_socktype = sockType; // SOCK_DGRAM or SOCK_STREAM
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	hints.ai_family = AF_UNSPEC ;

    if (SOCK_DGRAM == sockType)
    {
        hints.ai_protocol = IPPROTO_UDP;
    }
    else if (SOCK_STREAM == sockType)
    {
        hints.ai_protocol = IPPROTO_TCP;
    }

    struct addrinfo* servinfo = NULL;  // will point to the results

	char portStr[32];
	OVR_itoa(port, portStr, sizeof(portStr), 10);
	int errcode = getaddrinfo(hostAddress, portStr, &hints, &servinfo);

    if (0 != errcode)
    {
        OVR::LogError("getaddrinfo error: %s", gai_strerror(errcode));
    }

    OVR_ASSERT(servinfo);

    if (servinfo)
    {
        memcpy(&Addr6, servinfo->ai_addr, sizeof(Addr6));

        freeaddrinfo(servinfo);
    }
}

UInt16 SockAddr::GetPort()
{
	return htons(Addr6.sin6_port);
}

String SockAddr::ToString(bool writePort, char portDelineator) const
{
    char dest[INET6_ADDRSTRLEN + 1];

	int ret = getnameinfo((struct sockaddr*)&Addr6,
						  sizeof(struct sockaddr_in6),
						  dest,
						  INET6_ADDRSTRLEN,
						  NULL,
						  0,
						  NI_NUMERICHOST);
	if (ret != 0)
	{
		dest[0] = '\0';
	}

	if (writePort)
	{
		unsigned char ch[2];
		ch[0]=portDelineator;
		ch[1]=0;
		OVR_strcat(dest, 16, (const char*) ch);
		OVR_itoa(ntohs(Addr6.sin6_port), dest+strlen(dest), 16, 10);
	}

    return String(dest);
}
bool SockAddr::IsLocalhost() const
{
    static const unsigned char localhost_bytes[] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

    return memcmp(Addr6.sin6_addr.s6_addr, localhost_bytes, 16) == 0;
}
bool SockAddr::operator==( const SockAddr& right ) const
{
	return memcmp(&Addr6, &right.Addr6, sizeof(Addr6)) == 0;
}

bool SockAddr::operator!=( const SockAddr& right ) const
{
	return !(*this == right);
}

bool SockAddr::operator>( const SockAddr& right ) const
{
	return memcmp(&Addr6, &right.Addr6, sizeof(Addr6)) > 0;
}

bool SockAddr::operator<( const SockAddr& right ) const
{
	return memcmp(&Addr6, &right.Addr6, sizeof(Addr6)) < 0;
}


// Returns true on success
static bool SetSocketOptions(SocketHandle sock)
{
    bool failed = false;
    int sock_opt;
    int sockError = 0;

    // This doubles the max throughput rate
    sock_opt=1024*256;
    sockError = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) );
    if (sockError != 0)
    {
		int errsv = errno;
        OVR::LogError("[Socket] Failed SO_RCVBUF setsockopt, errno: %d", errsv);
        failed = true;
    }
    
    // This doesn't make much difference: 10% maybe
    // Not supported on console 2
    sock_opt=1024*16;
    sockError = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) );
    if (sockError != 0)
    {
		int errsv = errno;
        OVR::LogError("[Socket] Failed SO_SNDBUF setsockopt, errno: %d", errsv);
        failed = true;
    }

    // NOTE: This should be OVR_OS_BSD, not Mac.
#ifdef OVR_OS_MAC
    int value = 1;
    sockError = setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
    if (sockError != 0)
    {
        int errsv = errno;
        OVR::LogError("[Socket] Failed SO_NOSIGPIPE setsockopt, errno: %d", errsv);
        failed = true;
    }
#endif

    // Reuse address is only needed for posix platforms, as it is the default
    // on Windows platforms.
    int optval = 1;
    sockError = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    if (sockError != 0)
    {
		int errsv = errno;
        OVR::LogError("[Socket] Failed SO_REUSEADDR setsockopt, errno: %d", errsv);
        failed = true;
    }

    return !failed;
}

void _Ioctlsocket(SocketHandle sock, unsigned long nonblocking)
{
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) return; // return false
	if (nonblocking == 0)	{ flags &= ~O_NONBLOCK; }
	else					{ flags |= O_NONBLOCK;  }
	fcntl(sock, F_SETFL, flags);
}

static SocketHandle BindShared(int ai_family, int ai_socktype, BerkleyBindParameters *pBindParameters)
{
	SocketHandle sock;

	struct addrinfo hints;
	memset(&hints, 0, sizeof (addrinfo)); // make sure the struct is empty
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	struct addrinfo *servinfo=0, *aip;  // will point to the results
	char portStr[32];
	OVR_itoa(pBindParameters->Port, portStr, sizeof(portStr), 10);

    int errcode = 0;
	if (!pBindParameters->Address.IsEmpty())
		errcode = getaddrinfo(pBindParameters->Address.ToCStr(), portStr, &hints, &servinfo);
	else
		errcode = getaddrinfo(0, portStr, &hints, &servinfo);

    if (0 != errcode)
    {
        OVR::LogError("getaddrinfo error: %s", gai_strerror(errcode));
    }

	for (aip = servinfo; aip != NULL; aip = aip->ai_next)
	{
		// Open socket. The address type depends on what
		// getaddrinfo() gave us.
		sock = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
		if (sock != 0)
		{
            SetSocketOptions(sock);
			int ret = bind( sock, aip->ai_addr, (int) aip->ai_addrlen );
			if (ret>=0)
			{
				// The actual socket is always non-blocking
				// I control blocking or not using WSAEventSelect
				_Ioctlsocket(sock, 1);
                freeaddrinfo(servinfo);
				return sock;
			}
			else
			{
				close(sock);
			}
		}
	}

    if (servinfo) { freeaddrinfo(servinfo); }
	return INVALID_SOCKET;
}


//-----------------------------------------------------------------------------
// UDPSocket

UDPSocket::UDPSocket()
{
	RecvBuf = new UByte[RecvBufSize];
}

UDPSocket::~UDPSocket()
{
	delete[] RecvBuf;
}

SocketHandle UDPSocket::Bind(BerkleyBindParameters *pBindParameters)
{
	SocketHandle s = BindShared(AF_INET6, SOCK_DGRAM, pBindParameters);
	if (s < 0)
		return s;

	Close();
	TheSocket = s;

	return TheSocket;
}

void UDPSocket::OnRecv(SocketEvent_UDP* eventHandler, UByte* pData, int bytesRead, SockAddr* address)
{
	eventHandler->UDP_OnRecv(this, pData, bytesRead, address);
}

int UDPSocket::Send(const void* pData, int bytes, SockAddr* address)
{
    // NOTE: This should be OVR_OS_BSD
#ifdef OVR_OS_MAC
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif

	return (int)sendto(TheSocket, (const char*)pData, bytes, flags, (const sockaddr*)&address->Addr6, sizeof(address->Addr6));
}

void UDPSocket::Poll(SocketEvent_UDP *eventHandler)
{
	struct sockaddr_storage win32_addr;
	socklen_t fromlen;
	int bytesRead;

    // FIXME: Implement blocking poll wait for UDP

	// While some bytes are read,
	while (fromlen = sizeof(win32_addr), // Must set fromlen each time
		   bytesRead = (int)recvfrom(TheSocket, (char*)RecvBuf, RecvBufSize, 0, (sockaddr*)&win32_addr, &fromlen),
		   bytesRead > 0)
	{
		SockAddr address(&win32_addr); // Wrap address

		OnRecv(eventHandler, RecvBuf, bytesRead, &address);
	}
}


//-----------------------------------------------------------------------------
// TCPSocket

TCPSocket::TCPSocket()
{
	IsConnecting = false;
	IsListenSocket = false;
}
TCPSocket::TCPSocket(SocketHandle boundHandle, bool isListenSocket)
{
	TheSocket = boundHandle;
	IsListenSocket = isListenSocket;
	IsConnecting = false;
	SetSocketOptions(TheSocket);

	// The actual socket is always non-blocking
	_Ioctlsocket(TheSocket, 1);
}

TCPSocket::~TCPSocket()
{
}

void TCPSocket::OnRecv(SocketEvent_TCP* eventHandler, UByte* pData, int bytesRead)
{
	eventHandler->TCP_OnRecv(this, pData, bytesRead);
}

SocketHandle TCPSocket::Bind(BerkleyBindParameters* pBindParameters)
{	
	SocketHandle s = BindShared(AF_INET6, SOCK_STREAM, pBindParameters);
	if (s < 0)
		return s;

	Close();

    SetBlockingTimeout(pBindParameters->blockingTimeout);
    TheSocket = s;

	return TheSocket;
}

int TCPSocket::Listen()
{
    if (IsListenSocket)
    {
        return 0;
    }

	int i = listen(TheSocket, SOMAXCONN);
	if (i >= 0)
	{
		IsListenSocket = true;
	}

	return i;
}

int TCPSocket::Connect(SockAddr* address)
{
	int retval;

	retval = connect(TheSocket, (struct sockaddr *) &address->Addr6, sizeof(address->Addr6));
	if (retval < 0)
	{
		int errsv = errno;
        // EINPROGRESS should not be checked on windows but should
        // be checked on POSIX platforms.
		if (errsv == EWOULDBLOCK || errsv == EINPROGRESS)
		{
            IsConnecting = true;
            return 0;
		}

		OVR::LogText( "TCPSocket::Connect failed:Error code - %d\n", errsv );
	}

	return retval;
}

int TCPSocket::Send(const void* pData, int bytes)
{
	if (bytes <= 0)
	{
		return 0;
	}
	else
	{
		return (int)send(TheSocket, (const char*)pData, bytes, 0);
	}
}


//// TCPSocketPollState

TCPSocketPollState::TCPSocketPollState()
{
    FD_ZERO(&readFD);
    FD_ZERO(&exceptionFD);
    FD_ZERO(&writeFD);
    largestDescriptor = INVALID_SOCKET;
}

bool TCPSocketPollState::IsValid() const
{
    return largestDescriptor != INVALID_SOCKET;
}

void TCPSocketPollState::Add(TCPSocket* tcpSocket)
{
    if (!tcpSocket)
    {
        return;
    }

    SocketHandle handle = tcpSocket->GetSocketHandle();

    if (handle == INVALID_SOCKET)
    {
        return;
    }

    if (largestDescriptor == INVALID_SOCKET ||
        largestDescriptor < handle)
    {
        largestDescriptor = handle;
    }

    FD_SET(handle, &readFD);
    FD_SET(handle, &exceptionFD);

    if (tcpSocket->IsConnecting)
    {
        FD_SET(handle, &writeFD);
    }
}

bool TCPSocketPollState::Poll(long usec, long seconds)
{
    timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = (int)usec;

    return select(largestDescriptor + 1, &readFD, &writeFD, &exceptionFD, &tv) > 0;
}

void TCPSocketPollState::HandleEvent(TCPSocket* tcpSocket, SocketEvent_TCP* eventHandler)
{
    if (!tcpSocket || !eventHandler)
    {
        return;
    }

    SocketHandle handle = tcpSocket->GetSocketHandle();

    if (tcpSocket->IsConnecting && FD_ISSET(handle, &writeFD))
    {
        tcpSocket->IsConnecting = false;
        eventHandler->TCP_OnConnected(tcpSocket);
    }

    if (FD_ISSET(handle, &readFD))
    {
        if (!tcpSocket->IsListenSocket)
        {
            static const int BUFF_SIZE = 8096;
            char data[BUFF_SIZE];

            int bytesRead = (int)recv(handle, data, BUFF_SIZE, 0);
            if (bytesRead > 0)
            {
                tcpSocket->OnRecv(eventHandler, (UByte*)data, bytesRead);
            }
            else // Disconnection event:
            {
                tcpSocket->IsConnecting = false;
                eventHandler->TCP_OnClosed(tcpSocket);
            }
        }
        else
        {
            struct sockaddr_storage sockAddr;
            socklen_t sockAddrSize = sizeof(sockAddr);

            SocketHandle newSock = accept(handle, (sockaddr*)&sockAddr, (socklen_t*)&sockAddrSize);
            if (newSock > 0)
            {
                SockAddr sa(&sockAddr);
                eventHandler->TCP_OnAccept(tcpSocket, &sa, newSock);
            }
        }
    }

    if (FD_ISSET(handle, &exceptionFD))
    {
        tcpSocket->IsConnecting = false;
        eventHandler->TCP_OnClosed(tcpSocket);
    }
}


}} // namespace OVR::Net
