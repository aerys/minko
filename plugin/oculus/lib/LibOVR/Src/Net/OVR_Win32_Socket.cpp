/************************************************************************************

Filename    :   OVR_Win32_Socket.cpp
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

#include "OVR_Win32_Socket.h"
#include "../Kernel/OVR_Std.h"
#include "../Kernel/OVR_Allocator.h"
#include "../Kernel/OVR_Threads.h" // Thread::MSleep
#include "../Kernel/OVR_Log.h"

#include <Winsock2.h>

namespace OVR { namespace Net {


//-----------------------------------------------------------------------------
// WSAStartupSingleton

class WSAStartupSingleton
{
public:
	static void AddRef(void);
	static void Deref(void);

protected:
	static int RefCount;
};


// Local data
int WSAStartupSingleton::RefCount = 0;


// Implementation
void WSAStartupSingleton::AddRef()
{
	if (++RefCount == 1)
	{
		WSADATA winsockInfo;
		const int errCode = WSAStartup(MAKEWORD(2, 2), &winsockInfo);
		OVR_ASSERT(errCode == 0);

		// If an error code is returned
		if (errCode != 0)
		{
			LogError("{ERR-007w} [Socket] Unable to initialize Winsock %d", errCode);
		}
	}
}

void WSAStartupSingleton::Deref()
{
	OVR_ASSERT(RefCount > 0);

	if (RefCount > 0)
	{
		if (--RefCount == 0)
		{
			WSACleanup();
			RefCount = 0;
		}
	}
}


//-----------------------------------------------------------------------------
// BerkleySocket

void BerkleySocket::Close()
{
	if (TheSocket != INVALID_SOCKET)
	{
		closesocket(TheSocket);
		TheSocket = INVALID_SOCKET;
	}
}

int32_t BerkleySocket::GetSockname(SockAddr *pSockAddrOut)
{
	struct sockaddr_in6 sa;
	memset(&sa,0,sizeof(sa));
	int size = sizeof(sa);
	int32_t i = getsockname(TheSocket, (sockaddr*) &sa, &size);
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
	WSAStartupSingleton::AddRef();

    // Zero out the address to squelch static analysis tools
    ZeroMemory(&Addr6, sizeof(Addr6));
}

SockAddr::SockAddr(SockAddr* address)
{
	WSAStartupSingleton::AddRef();
	Set(&address->Addr6);
}

SockAddr::SockAddr(sockaddr_storage* storage)
{
	WSAStartupSingleton::AddRef();
	Set(storage);
}

SockAddr::SockAddr(sockaddr_in6* address)
{
	WSAStartupSingleton::AddRef();
	Set(address);
}

SockAddr::SockAddr(const char* hostAddress, uint16_t port, int sockType)
{
	WSAStartupSingleton::AddRef();
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

void SockAddr::Set(const char* hostAddress, uint16_t port, int sockType)
{
	memset(&Addr6, 0, sizeof(Addr6));

	struct addrinfo* servinfo = 0;  // will point to the results
	struct addrinfo hints;

	// make sure the struct is empty
	memset(&hints, 0, sizeof (addrinfo));

	hints.ai_socktype = sockType; // SOCK_DGRAM or SOCK_STREAM
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	hints.ai_family = AF_UNSPEC ;

    // FIXME See OVR_Unix_Socket implementation and man pages for getaddrinfo.
    //       ai_protocol is expecting to be either IPPROTO_UDP and IPPROTO_TCP.
    //       But this has been working on windows so I'm leaving it be for
    //       now instead of introducing another variable.
	hints.ai_protocol = IPPROTO_IPV6;

	char portStr[32];
	OVR_itoa(port, portStr, sizeof(portStr), 10);
	int errcode = getaddrinfo(hostAddress, portStr, &hints, &servinfo);

    if (0 != errcode)
    {
        OVR::LogError("{ERR-008w} getaddrinfo error: %s", gai_strerror(errcode));
    }

    OVR_ASSERT(0 != servinfo);

	memcpy(&Addr6, servinfo->ai_addr, sizeof(Addr6));

    freeaddrinfo(servinfo);
}

uint16_t SockAddr::GetPort()
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

static bool SetSocketOptions(SocketHandle sock)
{
    int result = 0;
	int sock_opt;

	// This doubles the max throughput rate
	sock_opt=1024*256;
    result |= setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)& sock_opt, sizeof (sock_opt));

	// Immediate hard close. Don't linger the socket, or recreating the socket quickly on Vista fails.
	// Fail with voice
	sock_opt=0;
    result |= setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)& sock_opt, sizeof (sock_opt));

	// This doesn't make much difference: 10% maybe
	// Not supported on console 2
	sock_opt=1024*16;
    result |= setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)& sock_opt, sizeof (sock_opt));

    // If all the setsockopt() returned 0 there were no failures, so return true for success, else false
    return result == 0;
}

void _Ioctlsocket(SocketHandle sock, unsigned long nonblocking)
{
	ioctlsocket( sock, FIONBIO, &nonblocking );
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
        OVR::LogError("{ERR-020w} getaddrinfo error: %s", gai_strerror(errcode));
    }

	for (aip = servinfo; aip != NULL; aip = aip->ai_next)
	{
		// Open socket. The address type depends on what
		// getaddrinfo() gave us.
		sock = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
        if (sock != INVALID_SOCKET)
		{
            if (bind(sock, aip->ai_addr, (int)aip->ai_addrlen) != SOCKET_ERROR)
			{
				// The actual socket is always non-blocking
				// I control blocking or not using WSAEventSelect
				_Ioctlsocket(sock, 1);
                freeaddrinfo(servinfo);
				return sock;
			}

				closesocket(sock);
			}
		}

    if (servinfo) { freeaddrinfo(servinfo); }
	return INVALID_SOCKET;
}


//-----------------------------------------------------------------------------
// UDPSocket

UDPSocket::UDPSocket()
{
	WSAStartupSingleton::AddRef();
	RecvBuf = new uint8_t[RecvBufSize];
}

UDPSocket::~UDPSocket()
{
	WSAStartupSingleton::Deref();
	delete[] RecvBuf;
}

SocketHandle UDPSocket::Bind(BerkleyBindParameters *pBindParameters)
{
	SocketHandle s = BindShared(AF_INET6, SOCK_DGRAM, pBindParameters);
	if (s == INVALID_SOCKET)
		return s;

	Close();
	TheSocket = s;
	SetSocketOptions(TheSocket);

	return TheSocket;
}

void UDPSocket::OnRecv(SocketEvent_UDP* eventHandler, uint8_t* pData, int bytesRead, SockAddr* address)
{
	eventHandler->UDP_OnRecv(this, pData, bytesRead, address);
}

int UDPSocket::Send(const void* pData, int bytes, SockAddr* address)
{
	return sendto(TheSocket, (const char*)pData, bytes, 0, (const sockaddr*)&address->Addr6, sizeof(address->Addr6));
}

void UDPSocket::Poll(SocketEvent_UDP *eventHandler)
{
	struct sockaddr_storage win32_addr;
	socklen_t fromlen;
	int bytesRead;

    // FIXME: Implement blocking poll wait for UDP

	// While some bytes are read,
	while (fromlen = sizeof(win32_addr), // Must set fromlen each time
		   bytesRead = recvfrom(TheSocket, (char*)RecvBuf, RecvBufSize, 0, (sockaddr*)&win32_addr, &fromlen),
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
	WSAStartupSingleton::AddRef();
}
TCPSocket::TCPSocket(SocketHandle boundHandle, bool isListenSocket)
{
	TheSocket = boundHandle;
	IsListenSocket = isListenSocket;
	IsConnecting = false;
	WSAStartupSingleton::AddRef();
	SetSocketOptions(TheSocket);

	// The actual socket is always non-blocking
	_Ioctlsocket(TheSocket, 1);
}

TCPSocket::~TCPSocket()
{
	WSAStartupSingleton::Deref();
}

void TCPSocket::OnRecv(SocketEvent_TCP* eventHandler, uint8_t* pData, int bytesRead)
{
	eventHandler->TCP_OnRecv(this, pData, bytesRead);
}

SocketHandle TCPSocket::Bind(BerkleyBindParameters* pBindParameters)
{	
	SocketHandle s = BindShared(AF_INET6, SOCK_STREAM, pBindParameters);
	if (s == INVALID_SOCKET)
		return s;

	Close();

    SetBlockingTimeout(pBindParameters->blockingTimeout);
    TheSocket = s;

    SetSocketOptions(TheSocket);

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
		DWORD dwIOError = WSAGetLastError();
		if (dwIOError == WSAEWOULDBLOCK)
		{
            IsConnecting = true;
            return 0;
		}

		printf( "TCPSocket::Connect failed:Error code - %d\n", dwIOError );
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
		return send(TheSocket, (const char*)pData, bytes, 0);
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
    tv.tv_usec = usec;

    return (int)select((int)largestDescriptor + 1, &readFD, &writeFD, &exceptionFD, &tv) > 0;
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

            int bytesRead = recv(handle, data, BUFF_SIZE, 0);
            if (bytesRead > 0)
            {
                tcpSocket->OnRecv(eventHandler, (uint8_t*)data, bytesRead);
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
            if (newSock != INVALID_SOCKET)
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
