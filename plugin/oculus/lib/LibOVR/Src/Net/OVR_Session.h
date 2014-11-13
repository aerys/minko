/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_Session.h
Content     :   One network session that provides connection/disconnection events.
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

#ifndef OVR_Session_h
#define OVR_Session_h

#include "OVR_Socket.h"
#include "OVR_PacketizedTCPSocket.h"
#include "../Kernel/OVR_Array.h"
#include "../Kernel/OVR_Threads.h"
#include "../Kernel/OVR_Atomic.h"
#include "../Kernel/OVR_RefCount.h"

namespace OVR { namespace Net {

class Session;


//-----------------------------------------------------------------------------
// Based on Semantic Versioning ( http://semver.org/ )
//
// Please update changelog below:
// 1.0.0 - [SDK 0.4.0] Initial version (July 21, 2014)
// 1.1.0 - Add Get/SetDriverMode_1, HMDCountUpdate_1
//         Version mismatch results (July 28, 2014)
//-----------------------------------------------------------------------------

static const uint16_t RPCVersion_Major = 1; // MAJOR version when you make incompatible API changes,
static const uint16_t RPCVersion_Minor = 1; // MINOR version when you add functionality in a backwards-compatible manner, and
static const uint16_t RPCVersion_Patch = 0; // PATCH version when you make backwards-compatible bug fixes.

// Client starts communication by sending its version number.
struct RPC_C2S_Hello
{
    RPC_C2S_Hello() :
        MajorVersion(0),
        MinorVersion(0),
        PatchVersion(0)
    {
    }

    String HelloString;

    // Client version info
    uint16_t MajorVersion, MinorVersion, PatchVersion;

    void Serialize(Net::BitStream* bs)
    {
        bs->Write(HelloString);
        bs->Write(MajorVersion);
        bs->Write(MinorVersion);
        bs->Write(PatchVersion);
    }

    bool Deserialize(Net::BitStream* bs)
    {
        bs->Read(HelloString);
        bs->Read(MajorVersion);
        bs->Read(MinorVersion);
        return bs->Read(PatchVersion);
    }

    static void Generate(Net::BitStream* bs);

    bool Validate();
};

// Server responds with an authorization accepted message, including the server's version number
struct RPC_S2C_Authorization
{
    RPC_S2C_Authorization() :
        MajorVersion(0),
        MinorVersion(0),
        PatchVersion(0)
    {
    }

    String AuthString;

    // Server version info
    uint16_t MajorVersion, MinorVersion, PatchVersion;

    void Serialize(Net::BitStream* bs)
    {
        bs->Write(AuthString);
        bs->Write(MajorVersion);
        bs->Write(MinorVersion);
        bs->Write(PatchVersion);
    }

    bool Deserialize(Net::BitStream* bs)
    {
        bs->Read(AuthString);
        bs->Read(MajorVersion);
        bs->Read(MinorVersion);
        return bs->Read(PatchVersion);
    }

    static void Generate(Net::BitStream* bs, String errorString = "");

    bool Validate();
};


//-----------------------------------------------------------------------------
// Result of a session function
enum SessionResult
{
	SessionResult_OK,
	SessionResult_BindFailure,
	SessionResult_ListenFailure,
	SessionResult_ConnectFailure,
    SessionResult_ConnectInProgress,
    SessionResult_AlreadyConnected,
};


//-----------------------------------------------------------------------------
// Connection state
enum EConnectionState
{
    State_Zombie,          // Disconnected

    // Client-only:
    Client_Connecting,     // Waiting for TCP connection
    Client_ConnectedWait,  // Connected! Waiting for server to authorize

    // Server-only:
    Server_ConnectedWait,  // Connected! Waiting for client handshake

    State_Connected        // Connected
};


//-----------------------------------------------------------------------------
// Generic connection over any transport
class Connection : public RefCountBase<Connection>
{
public:
    Connection() :
        Transport(TransportType_None),
        State(State_Zombie),
        RemoteMajorVersion(0),
        RemoteMinorVersion(0),
        RemotePatchVersion(0)
    {
    }
	virtual ~Connection() // Allow delete from base
    {
    }

public:
    virtual void SetState(EConnectionState s) {State = s;}

    TransportType    Transport;
    EConnectionState State;

    // Version number read from remote host just before connection completes
    int              RemoteMajorVersion;
    int              RemoteMinorVersion;
    int              RemotePatchVersion;
};


//-----------------------------------------------------------------------------
// Generic network connection over any network transport
class NetworkConnection : public Connection
{
protected:
    NetworkConnection()
	{
	}
    virtual ~NetworkConnection()
    {
    }

public:
    virtual void SetState(EConnectionState s)
    {
        if (s != State)
        {
            Mutex::Locker locker(&StateMutex);

            if (s != State)
            {
                State = s;

                if (State != Client_Connecting)
                {
                    ConnectingWait.NotifyAll();
                }
            }
        }
    }

    void WaitOnConnecting()
    {
        Mutex::Locker locker(&StateMutex);

        while (State == Client_Connecting)
        {
            ConnectingWait.Wait(&StateMutex);
        }
    }

	SockAddr      Address;
    Mutex         StateMutex;
    WaitCondition ConnectingWait;
};


//-----------------------------------------------------------------------------
// TCP Connection
class TCPConnection : public NetworkConnection
{
public:
    TCPConnection()
    {
    }
    virtual ~TCPConnection()
    {
    }

public:
	Ptr<TCPSocket> pSocket;
};


//-----------------------------------------------------------------------------
// Packetized TCP Connection
class PacketizedTCPConnection : public TCPConnection
{
public:
	PacketizedTCPConnection()
    {
        Transport = TransportType_PacketizedTCP;
    }
    virtual ~PacketizedTCPConnection()
    {
    }
};


//-----------------------------------------------------------------------------
// Generic socket listener description
class ListenerDescription
{
public:
    ListenerDescription() :
        Transport(TransportType_None)
    {
    }

    TransportType Transport;
};


//-----------------------------------------------------------------------------
// Description for a Berkley socket listener
class BerkleyListenerDescription : public ListenerDescription
{
public:
	static const int DefaultMaxIncomingConnections =  64;
	static const int DefaultMaxConnections         = 128;

	BerkleyListenerDescription() :
		MaxIncomingConnections(DefaultMaxIncomingConnections),
		MaxConnections(DefaultMaxConnections)
	{
	}

	Ptr<BerkleySocket> BoundSocketToListenWith;
    int                MaxIncomingConnections;
    int                MaxConnections;
};


//-----------------------------------------------------------------------------
// Receive payload
struct ReceivePayload
{
	Connection* pConnection; // Source connection
	uint8_t*      pData;       // Pointer to data received
	int         Bytes;       // Number of bytes of data received
};

//-----------------------------------------------------------------------------
// Broadcast parameters
class BroadcastParameters
{
public:
    BroadcastParameters() :
        pData(NULL),
        Bytes(0)
    {
    }

    BroadcastParameters(const void* _pData, int _bytes) :
        pData(_pData),
        Bytes(_bytes)
    {
    }

public:
    const void*     pData;       // Pointer to data to send
    int             Bytes;       // Number of bytes of data received
};

//-----------------------------------------------------------------------------
// Send parameters
class SendParameters
{
public:
	SendParameters() :
		pData(NULL),
		Bytes(0)
	{
	}
	SendParameters(Ptr<Connection> _pConnection, const void* _pData, int _bytes) :
		pConnection(_pConnection),
		pData(_pData),
		Bytes(_bytes)
	{
	}

public:
	Ptr<Connection> pConnection; // Connection to use
	const void*     pData;       // Pointer to data to send
	int             Bytes;       // Number of bytes of data received
};


//-----------------------------------------------------------------------------
// Parameters to connect
struct ConnectParameters
{
public:
	ConnectParameters() :
		Transport(TransportType_None)
	{
	}

	TransportType Transport;
};

struct ConnectParametersBerkleySocket : public ConnectParameters
{
	SockAddr           RemoteAddress;
	Ptr<BerkleySocket> BoundSocketToConnectWith;
    bool                Blocking;

	ConnectParametersBerkleySocket()
    {
    }

	ConnectParametersBerkleySocket(Socket* s, SockAddr* addr) :
        RemoteAddress(*addr)
    {
        BoundSocketToConnectWith = (BerkleySocket*)s;
    }
};


//-----------------------------------------------------------------------------
// Listener receive result
enum ListenerReceiveResult
{
	/// The SessionListener used this message and it shouldn't be given to the user.
	LRR_RETURN = 0,

	/// The SessionListener is going to hold on to this message.  Do not deallocate it but do not pass it to other plugins either.
	LRR_BREAK,

    /// This message will be processed by other SessionListeners, and at last by the user.
    LRR_CONTINUE,
};


//-----------------------------------------------------------------------------
// SessionListener

// Callback interface for network events such as connecting, disconnecting, getting data, independent of the transport medium
class SessionListener
{
public:
	// Data events
    virtual void OnReceive(ReceivePayload* pPayload, ListenerReceiveResult* lrrOut) { OVR_UNUSED2(pPayload, lrrOut);  }

	// Connection was closed
    virtual void OnDisconnected(Connection* conn) = 0;

	// Connection was created (some data was exchanged to verify protocol compatibility too)
    virtual void OnConnected(Connection* conn) = 0;

    // Server accepted client
    virtual void OnNewIncomingConnection(Connection* conn)     { OnConnected(conn); }
    // Client was accepted
    virtual void OnConnectionRequestAccepted(Connection* conn) { OnConnected(conn); }

    // Connection attempt failed for some reason
    virtual void OnConnectionAttemptFailed(Connection* conn)   { OnDisconnected(conn); }

    // Incompatible protocol
    virtual void OnIncompatibleProtocol(Connection* conn)      { OnConnectionAttemptFailed(conn); }
    // Disconnected during initial handshake
    virtual void OnHandshakeAttemptFailed(Connection* conn)    { OnConnectionAttemptFailed(conn); }

	// Other
    virtual void OnAddedToSession(Session* session)            { OVR_UNUSED(session); }
    virtual void OnRemovedFromSession(Session* session)        { OVR_UNUSED(session); }
};


//-----------------------------------------------------------------------------
// Session

//  Interface for network events such as listening on a socket, sending data, connecting, and disconnecting. Works independently of the transport medium and also implements loopback
class Session : public SocketEvent_TCP, public NewOverrideBase
{
public:
    Session() :
        HasLoopbackListener(false)
    {
    }
    virtual ~Session()
    {
    }

	virtual SessionResult Listen(ListenerDescription* pListenerDescription);
	virtual SessionResult Connect(ConnectParameters* cp);
	virtual int           Send(SendParameters* payload);
    virtual void          Broadcast(BroadcastParameters* payload);
    virtual void          Poll(bool listeners = true);
	virtual void          AddSessionListener(SessionListener* se);
	virtual void          RemoveSessionListener(SessionListener* se);
    virtual SInt32        GetActiveSocketsCount();

    // Packetized TCP convenience functions
    virtual SessionResult ListenPTCP(BerkleyBindParameters* bbp);
    virtual SessionResult ConnectPTCP(BerkleyBindParameters* bbp, SockAddr* RemoteAddress, bool blocking);

    // Closes all the sockets; useful for interrupting the socket polling during shutdown
    void            Shutdown();

    // Get count of successful connections (past handshake point)
    int             GetConnectionCount() const
    {
        return FullConnections.GetSizeI();
    }
    Ptr<Connection> GetConnectionAtIndex(int index);

protected:
	virtual Ptr<Connection> AllocConnection(TransportType transportType);

    Lock SocketListenersLock, ConnectionsLock, SessionListenersLock;
    bool                      HasLoopbackListener; // Has loopback listener installed?
	Array< Ptr<TCPSocket> >   SocketListeners;     // List of active sockets
    Array< Ptr<Connection> >  AllConnections;      // List of active connections stuck at the versioning handshake
    Array< Ptr<Connection> >  FullConnections;     // List of active connections past the versioning handshake
    Array< SessionListener* > SessionListeners;    // List of session listeners

    // Tools
    Ptr<PacketizedTCPConnection> findConnectionBySocket(Array< Ptr<Connection> >& connectionArray, Socket* s, int *connectionIndex = NULL); // Call with ConnectionsLock held
    Ptr<PacketizedTCPConnection> findConnectionBySockAddr(SockAddr* address); // Call with ConnectionsLock held
    int                   invokeSessionListeners(ReceivePayload*);
    void                  invokeSessionEvent(void(SessionListener::*f)(Connection*), Connection* pConnection);

	// TCP
	virtual void          TCP_OnRecv(Socket* pSocket, uint8_t* pData, int bytesRead);
	virtual void          TCP_OnClosed(TCPSocket* pSocket);
	virtual void          TCP_OnAccept(TCPSocket* pListener, SockAddr* pSockAddr, SocketHandle newSock);
	virtual void          TCP_OnConnected(TCPSocket* pSocket);
};


}} // OVR::Net

#endif
