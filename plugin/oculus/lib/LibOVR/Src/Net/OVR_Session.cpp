/************************************************************************************

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

#include "OVR_Session.h"
#include "OVR_PacketizedTCPSocket.h"
#include "../Kernel/OVR_Log.h"
#include "../Service/Service_NetSessionCommon.h"

namespace OVR { namespace Net {


//-----------------------------------------------------------------------------
// Protocol

static const char* OfficialHelloString = "OculusVR_Hello";
static const char* OfficialAuthorizedString = "OculusVR_Authorized";

void RPC_C2S_Hello::Generate(Net::BitStream* bs)
{
    RPC_C2S_Hello hello;
    hello.HelloString = OfficialHelloString;
    hello.MajorVersion = RPCVersion_Major;
    hello.MinorVersion = RPCVersion_Minor;
    hello.PatchVersion = RPCVersion_Patch;
    hello.Serialize(bs);
}

bool RPC_C2S_Hello::Validate()
{
    return MajorVersion == RPCVersion_Major &&
           MinorVersion <= RPCVersion_Minor &&
           HelloString.CompareNoCase(OfficialHelloString) == 0;
}

void RPC_S2C_Authorization::Generate(Net::BitStream* bs, String errorString)
{
    RPC_S2C_Authorization auth;
    if (errorString.IsEmpty())
    {
        auth.AuthString = OfficialAuthorizedString;
    }
    else
    {
        auth.AuthString = errorString;
    }
    auth.MajorVersion = RPCVersion_Major;
    auth.MinorVersion = RPCVersion_Minor;
    auth.PatchVersion = RPCVersion_Patch;
    auth.Serialize(bs);
}

bool RPC_S2C_Authorization::Validate()
{
    return AuthString.CompareNoCase(OfficialAuthorizedString) == 0;
}


//-----------------------------------------------------------------------------
// Session

void Session::Shutdown()
{
    {
        Lock::Locker locker(&SocketListenersLock);

        const int count = SocketListeners.GetSizeI();
        for (int i = 0; i < count; ++i)
        {
            SocketListeners[i]->Close();
        }
    }

    Lock::Locker locker(&ConnectionsLock);

    const int count = AllConnections.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        Connection* arrayItem = AllConnections[i].GetPtr();

        if (arrayItem->Transport == TransportType_PacketizedTCP)
        {
            PacketizedTCPConnection* ptcp = (PacketizedTCPConnection*)arrayItem;

            ptcp->pSocket->Close();
        }
    }
}

SessionResult Session::Listen(ListenerDescription* pListenerDescription)
{
	if (pListenerDescription->Transport == TransportType_PacketizedTCP)
	{
		BerkleyListenerDescription* bld = (BerkleyListenerDescription*)pListenerDescription;
		TCPSocket* tcpSocket = (TCPSocket*)bld->BoundSocketToListenWith.GetPtr();

        if (tcpSocket->Listen() < 0)
        {
            return SessionResult_ListenFailure;
        }

		Lock::Locker locker(&SocketListenersLock);
        SocketListeners.PushBack(tcpSocket);
	}
    else if (pListenerDescription->Transport == TransportType_Loopback)
	{
		HasLoopbackListener = true;
	}
    else
    {
        OVR_ASSERT(false);
    }

	return SessionResult_OK;
}

SessionResult Session::Connect(ConnectParameters *cp)
{
    if (cp->Transport == TransportType_PacketizedTCP)
    {
        ConnectParametersBerkleySocket* cp2 = (ConnectParametersBerkleySocket*)cp;
        Ptr<PacketizedTCPConnection> c;

        {
            Lock::Locker locker(&ConnectionsLock);

            int connIndex;
            Ptr<PacketizedTCPConnection> conn = findConnectionBySocket(AllConnections, cp2->BoundSocketToConnectWith, &connIndex);
            if (conn)
            {
                return SessionResult_AlreadyConnected;
            }

            TCPSocketBase* tcpSock = (TCPSocketBase*)cp2->BoundSocketToConnectWith.GetPtr();

            int ret = tcpSock->Connect(&cp2->RemoteAddress);
            if (ret < 0)
            {
                return SessionResult_ConnectFailure;
            }

            Ptr<Connection> newConnection = AllocConnection(cp2->Transport);
            if (!newConnection)
            {
                return SessionResult_ConnectFailure;
            }

            c = (PacketizedTCPConnection*)newConnection.GetPtr();
            c->pSocket = (TCPSocket*) cp2->BoundSocketToConnectWith.GetPtr();
            c->Address = cp2->RemoteAddress;
            c->Transport = cp2->Transport;
            c->SetState(Client_Connecting);

            AllConnections.PushBack(c);

        }

        if (cp2->Blocking)
        {
            c->WaitOnConnecting();
        }

        if (c->State == State_Connected)
        {
            return SessionResult_OK;
        }
        else if (c->State == Client_Connecting)
        {
            return SessionResult_ConnectInProgress;
        }
        else
        {
            return SessionResult_ConnectFailure;
        }
    }
    else if (cp->Transport == TransportType_Loopback)
	{
		if (HasLoopbackListener)
		{
            Ptr<Connection> c = AllocConnection(cp->Transport);
            if (!c)
            {
                return SessionResult_ConnectFailure;
            }

            c->Transport = cp->Transport;
            c->SetState(State_Connected);

            {
                Lock::Locker locker(&ConnectionsLock);
                AllConnections.PushBack(c);
            }

			invokeSessionEvent(&SessionListener::OnConnectionRequestAccepted, c);
		}
		else
		{
            OVR_ASSERT(false);
		}
	}
    else
    {
        OVR_ASSERT(false);
    }

	return SessionResult_OK;
}

SessionResult Session::ListenPTCP(OVR::Net::BerkleyBindParameters *bbp)
{
	Ptr<PacketizedTCPSocket> listenSocket = *new OVR::Net::PacketizedTCPSocket();
    if (listenSocket->Bind(bbp) == INVALID_SOCKET)
    {
        return SessionResult_BindFailure;
    }

	BerkleyListenerDescription bld;
	bld.BoundSocketToListenWith = listenSocket.GetPtr();
    bld.Transport = TransportType_PacketizedTCP;

    return Listen(&bld);
}

SessionResult Session::ConnectPTCP(OVR::Net::BerkleyBindParameters* bbp, SockAddr* RemoteAddress, bool blocking)
{
	ConnectParametersBerkleySocket cp;
    cp.RemoteAddress = RemoteAddress;
    cp.Transport = TransportType_PacketizedTCP;
    cp.Blocking = blocking;
    Ptr<PacketizedTCPSocket> connectSocket = *new PacketizedTCPSocket();

	cp.BoundSocketToConnectWith = connectSocket.GetPtr();
    if (connectSocket->Bind(bbp) == INVALID_SOCKET)
    {
        return SessionResult_BindFailure;
    }

	return Connect(&cp);
}

Ptr<PacketizedTCPConnection> Session::findConnectionBySockAddr(SockAddr* address)
{
    const int count = AllConnections.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        Connection* arrayItem = AllConnections[i].GetPtr();

        if (arrayItem->Transport == TransportType_PacketizedTCP)
        {
            PacketizedTCPConnection* conn = (PacketizedTCPConnection*)arrayItem;

            if (conn->Address == *address)
            {
                return conn;
            }
        }
    }

    return 0;
}

int Session::Send(SendParameters *payload)
{
	if (payload->pConnection->Transport == TransportType_Loopback)
	{
		Lock::Locker locker(&SessionListenersLock);

        const int count = SessionListeners.GetSizeI();
        for (int i = 0; i < count; ++i)
		{
			SessionListener* sl = SessionListeners[i];

            // FIXME: This looks like it needs to be reviewed at some point..
			ReceivePayload rp;
			rp.Bytes = payload->Bytes;
			rp.pConnection = payload->pConnection;
			rp.pData = (uint8_t*)payload->pData; // FIXME
			ListenerReceiveResult lrr = LRR_CONTINUE;
			sl->OnReceive(&rp, &lrr);
            if (lrr == LRR_RETURN)
            {
                return payload->Bytes;
            }
			else if (lrr == LRR_BREAK)
			{
				break;
			}	
		}

		return payload->Bytes;
	}
    else if (payload->pConnection->Transport == TransportType_PacketizedTCP)
	{
		PacketizedTCPConnection* conn = (PacketizedTCPConnection*)payload->pConnection.GetPtr();

        return conn->pSocket->Send(payload->pData, payload->Bytes);
	}
    else
    {
        OVR_ASSERT(false);
    }

    return 0;
}
void Session::Broadcast(BroadcastParameters *payload)
{
    SendParameters sp;
    sp.Bytes=payload->Bytes;
    sp.pData=payload->pData;

    {
        Lock::Locker locker(&ConnectionsLock);

        const int connectionCount = FullConnections.GetSizeI();
        for (int i = 0; i < connectionCount; ++i)
        {
            sp.pConnection = FullConnections[i];
            Send(&sp);
        }    
    }
}
void Session::Poll(bool listeners)
{
	Array< Ptr< Net::TCPSocket > > allBlockingTcpSockets;

	if (listeners)
	{
		Lock::Locker locker(&SocketListenersLock);

        const int listenerCount = SocketListeners.GetSizeI();
        for (int i = 0; i < listenerCount; ++i)
		{
            allBlockingTcpSockets.PushBack(SocketListeners[i]);
		}
	}

    {
        Lock::Locker locker(&ConnectionsLock);

        const int connectionCount = AllConnections.GetSizeI();
        for (int i = 0; i < connectionCount; ++i)
        {
            Connection* arrayItem = AllConnections[i].GetPtr();

            if (arrayItem->Transport == TransportType_PacketizedTCP)
            {
                PacketizedTCPConnection* ptcp = (PacketizedTCPConnection*)arrayItem;

                allBlockingTcpSockets.PushBack(ptcp->pSocket);
            }
            else
            {
                OVR_ASSERT(false);
            }
        }
    }

    const int count = allBlockingTcpSockets.GetSizeI();
	if (count > 0)
	{
        TCPSocketPollState state;

        // Add all the sockets for polling,
        for (int i = 0; i < count; ++i)
        {
            Net::TCPSocket* sock = allBlockingTcpSockets[i].GetPtr();

            // If socket handle is invalid,
            if (sock->GetSocketHandle() == INVALID_SOCKET)
            {
                OVR_DEBUG_LOG(("[Session] Detected an invalid socket handle - Treating it as a disconnection."));
                sock->IsConnecting = false;
                TCP_OnClosed(sock);
            }
            else
            {
                state.Add(sock);
            }
        }

        // If polling returns with an event,
        if (state.Poll(allBlockingTcpSockets[0]->GetBlockingTimeoutUsec(), allBlockingTcpSockets[0]->GetBlockingTimeoutSec()))
        {
            // Handle any events for each socket
            for (int i = 0; i < count; ++i)
            {
                state.HandleEvent(allBlockingTcpSockets[i], this);
            }
        }
	}
}

void Session::AddSessionListener(SessionListener* se)
{
	Lock::Locker locker(&SessionListenersLock);

    const int count = SessionListeners.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        if (SessionListeners[i] == se)
        {
            // Already added
            return;
        }
    }

    SessionListeners.PushBack(se);
	se->OnAddedToSession(this);
}

void Session::RemoveSessionListener(SessionListener* se)
{
	Lock::Locker locker(&SessionListenersLock);

    const int count = SessionListeners.GetSizeI();
	for (int i = 0; i < count; ++i)
	{
        if (SessionListeners[i] == se)
		{
            se->OnRemovedFromSession(this);

            SessionListeners.RemoveAtUnordered(i);
            break;
		}
	}
}
SInt32 Session::GetActiveSocketsCount()
{
    Lock::Locker locker1(&SocketListenersLock);
    Lock::Locker locker2(&ConnectionsLock);
    return SocketListeners.GetSize() + AllConnections.GetSize()>0;
}
Ptr<Connection> Session::AllocConnection(TransportType transport)
{
    switch (transport)
    {
    case TransportType_Loopback:      return *new Connection();
    case TransportType_TCP:           return *new TCPConnection();
    case TransportType_PacketizedTCP: return *new PacketizedTCPConnection();
    default:
        OVR_ASSERT(false);
        break;
    }

    return NULL;
}

Ptr<PacketizedTCPConnection> Session::findConnectionBySocket(Array< Ptr<Connection> >& connectionArray, Socket* s, int *connectionIndex)
{
    const int count = connectionArray.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        Connection* arrayItem = connectionArray[i].GetPtr();

        if (arrayItem->Transport == TransportType_PacketizedTCP)
        {
            PacketizedTCPConnection* ptc = (PacketizedTCPConnection*)arrayItem;

            if (ptc->pSocket == s)
            {
                if (connectionIndex)
                {
                    *connectionIndex = i;
                }
                return ptc;
            }
        }
    }

    return NULL;
}

int Session::invokeSessionListeners(ReceivePayload* rp)
{
    Lock::Locker locker(&SessionListenersLock);

    const int count = SessionListeners.GetSizeI();
    for (int j = 0; j < count; ++j)
    {
        ListenerReceiveResult lrr = LRR_CONTINUE;
        SessionListeners[j]->OnReceive(rp, &lrr);

        if (lrr == LRR_RETURN || lrr == LRR_BREAK)
        {
            break;
        }
    }

    return rp->Bytes;
}

void Session::TCP_OnRecv(Socket* pSocket, uint8_t* pData, int bytesRead)
{
	Lock::Locker locker(&ConnectionsLock);

    // Look for the connection in the full connection list first
    int connIndex;
    PacketizedTCPConnection* conn = findConnectionBySocket(AllConnections, pSocket, &connIndex);
    if (conn)
    {
        if (conn->State == State_Connected)
        {
            ReceivePayload rp;
            rp.Bytes = bytesRead;
            rp.pConnection = conn;
            rp.pData = pData;

            // Call listeners
            invokeSessionListeners(&rp);
        }
        else if (conn->State == Client_ConnectedWait)
        {
            // Check the version data from the message
            BitStream bsIn((char*)pData, bytesRead, false);

            RPC_S2C_Authorization auth;
            if (!auth.Deserialize(&bsIn) ||
                !auth.Validate())
            {
                LogError("{ERR-001} [Session] REJECTED: OVRService did not authorize us: %s", auth.AuthString.ToCStr());

                conn->SetState(State_Zombie);
                invokeSessionEvent(&SessionListener::OnIncompatibleProtocol, conn);
            }
            else
            {
                // Read remote version
                conn->RemoteMajorVersion = auth.MajorVersion;
                conn->RemoteMinorVersion = auth.MinorVersion;
                conn->RemotePatchVersion = auth.PatchVersion;

                // Mark as connected
                conn->SetState(State_Connected);
                FullConnections.PushBack(conn);
                invokeSessionEvent(&SessionListener::OnConnectionRequestAccepted, conn);
            }
        }
        else if (conn->State == Server_ConnectedWait)
        {
            // Check the version data from the message
            BitStream bsIn((char*)pData, bytesRead, false);

            RPC_C2S_Hello hello;
            if (!hello.Deserialize(&bsIn) ||
                !hello.Validate())
            {
                LogError("{ERR-002} [Session] REJECTED: Rift application is using an incompatible version %d.%d.%d (my version=%d.%d.%d)",
                         hello.MajorVersion, hello.MinorVersion, hello.PatchVersion,
                         RPCVersion_Major, RPCVersion_Minor, RPCVersion_Patch);

                conn->SetState(State_Zombie);

                // Send auth response
                BitStream bsOut;
                RPC_S2C_Authorization::Generate(&bsOut, "Incompatible protocol version.  Please make sure your OVRService and SDK are both up to date.");
                conn->pSocket->Send(bsOut.GetData(), bsOut.GetNumberOfBytesUsed());
            }
            else
            {
                // Read remote version
                conn->RemoteMajorVersion = hello.MajorVersion;
                conn->RemoteMinorVersion = hello.MinorVersion;
                conn->RemotePatchVersion = hello.PatchVersion;

                // Send auth response
                BitStream bsOut;
                RPC_S2C_Authorization::Generate(&bsOut);
                conn->pSocket->Send(bsOut.GetData(), bsOut.GetNumberOfBytesUsed());

                // Mark as connected
                conn->SetState(State_Connected);
                FullConnections.PushBack(conn);
                invokeSessionEvent(&SessionListener::OnNewIncomingConnection, conn);

            }
        }
        else
        {
            OVR_ASSERT(false);
        }
    }
}

void Session::TCP_OnClosed(TCPSocket* s)
{
	Lock::Locker locker(&ConnectionsLock);

    // If found in the full connection list,
    int connIndex;
    Ptr<PacketizedTCPConnection> conn = findConnectionBySocket(AllConnections, s, &connIndex);
    if (conn)
    {
        AllConnections.RemoveAtUnordered(connIndex);

        // If in the full connection list,
        if (findConnectionBySocket(FullConnections, s, &connIndex))
        {
            FullConnections.RemoveAtUnordered(connIndex);
        }

        // Generate an appropriate event for the current state
        switch (conn->State)
        {
        case Client_Connecting:
            invokeSessionEvent(&SessionListener::OnConnectionAttemptFailed, conn);
            break;
        case Client_ConnectedWait:
        case Server_ConnectedWait:
            invokeSessionEvent(&SessionListener::OnHandshakeAttemptFailed, conn);
            break;
        case State_Connected:
        case State_Zombie:
            invokeSessionEvent(&SessionListener::OnDisconnected, conn);
            break;
        default:
            OVR_ASSERT(false);
            break;
        }

        conn->SetState(State_Zombie);
    }
}

void Session::TCP_OnAccept(TCPSocket* pListener, SockAddr* pSockAddr, SocketHandle newSock)
{
    OVR_UNUSED(pListener);
	OVR_ASSERT(pListener->Transport == TransportType_PacketizedTCP);


	Ptr<PacketizedTCPSocket> newSocket = *new PacketizedTCPSocket(newSock, false);
    // If pSockAddr is not localhost, then close newSock
    if (pSockAddr->IsLocalhost()==false)
    {
        newSocket->Close();
        return;
    }

	if (newSocket)
	{
		Ptr<Connection> b = AllocConnection(TransportType_PacketizedTCP);
		Ptr<PacketizedTCPConnection> c = (PacketizedTCPConnection*)b.GetPtr();
		c->pSocket = newSocket;
		c->Address = *pSockAddr;
        c->State = Server_ConnectedWait;

        {
            Lock::Locker locker(&ConnectionsLock);
            AllConnections.PushBack(c);
        }

        // Server does not send the first packet.  It waits for the client to send its version
	}
}

void Session::TCP_OnConnected(TCPSocket *s)
{
    Lock::Locker locker(&ConnectionsLock);

    // If connection was found,
    PacketizedTCPConnection* conn = findConnectionBySocket(AllConnections, s);
    if (conn)
    {
        OVR_ASSERT(conn->State == Client_Connecting);

        // Send hello message
        BitStream bsOut;
        RPC_C2S_Hello::Generate(&bsOut);
        conn->pSocket->Send(bsOut.GetData(), bsOut.GetNumberOfBytesUsed());

        // Just update state but do not generate any notifications yet
        conn->State = Client_ConnectedWait;
    }
}

void Session::invokeSessionEvent(void(SessionListener::*f)(Connection*), Connection* conn)
{
    Lock::Locker locker(&SessionListenersLock);

    const int count = SessionListeners.GetSizeI();
    for (int i = 0; i < count; ++i)
    {
        (SessionListeners[i]->*f)(conn);
    }
}

Ptr<Connection> Session::GetConnectionAtIndex(int index)
{
    Lock::Locker locker(&ConnectionsLock);

    const int count = FullConnections.GetSizeI();

    if (index < count)
    {
        return FullConnections[index];
    }

    return NULL;
}


}} // OVR::Net
