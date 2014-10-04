/************************************************************************************

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

#include "OVR_PacketizedTCPSocket.h"

namespace OVR { namespace Net {


//-----------------------------------------------------------------------------
// Constants

static const int LENGTH_FIELD_BYTES = 4;


//-----------------------------------------------------------------------------
// PacketizedTCPSocket

PacketizedTCPSocket::PacketizedTCPSocket()
{
	pRecvBuff = 0;
	pRecvBuffSize = 0;
	Transport = TransportType_PacketizedTCP;
}

PacketizedTCPSocket::PacketizedTCPSocket(SocketHandle _sock, bool isListenSocket) : PacketizedTCPSocketBase(_sock, isListenSocket)
{
	pRecvBuff = 0;
	pRecvBuffSize = 0;
	Transport = TransportType_PacketizedTCP;
}

PacketizedTCPSocket::~PacketizedTCPSocket()
{
	OVR_FREE(pRecvBuff);
}

int PacketizedTCPSocket::Send(const void* pData, int bytes)
{
    Lock::Locker locker(&sendLock);

	if (bytes <= 0)
	{
		return -1;
	}

	// Convert length to 4 endian-neutral bytes
	uint32_t lengthWord = bytes;
	uint8_t lengthBytes[LENGTH_FIELD_BYTES] = {
		(uint8_t)lengthWord,
		(uint8_t)(lengthWord >> 8),
		(uint8_t)(lengthWord >> 16),
		(uint8_t)(lengthWord >> 24)
	};

	int s = PacketizedTCPSocketBase::Send(lengthBytes, LENGTH_FIELD_BYTES);
	if (s > 0)
	{
		return PacketizedTCPSocketBase::Send(pData,bytes);
	}
	else
	{
		return s;
	}
}

int PacketizedTCPSocket::SendAndConcatenate(const void** pDataArray, int* dataLengthArray, int arrayCount)
{
    Lock::Locker locker(&sendLock);

    if (arrayCount == 0)
		return 0;

	int totalBytes = 0;
	for (int i = 0; i < arrayCount; i++)
		totalBytes += dataLengthArray[i];

	// Convert length to 4 endian-neutral bytes
	uint32_t lengthWord = totalBytes;
	uint8_t lengthBytes[LENGTH_FIELD_BYTES] = {
		(uint8_t)lengthWord,
		(uint8_t)(lengthWord >> 8),
		(uint8_t)(lengthWord >> 16),
		(uint8_t)(lengthWord >> 24)
	};

	int s = PacketizedTCPSocketBase::Send(lengthBytes, LENGTH_FIELD_BYTES);
	if (s > 0)
	{
		for (int i = 0; i < arrayCount; i++)
		{
			PacketizedTCPSocketBase::Send(pDataArray[i], dataLengthArray[i]);
		}
	}

	return s;
}

void PacketizedTCPSocket::OnRecv(SocketEvent_TCP* eventHandler, uint8_t* pData, int bytesRead)
{
	uint8_t* dataSource;
	int dataSourceSize;

	recvBuffLock.DoLock();

	if (pRecvBuff == NULL)
	{
		dataSource = pData;
		dataSourceSize = bytesRead;
	}
	else
	{
		pRecvBuff = (uint8_t*)OVR_REALLOC(pRecvBuff, bytesRead + pRecvBuffSize);
		memcpy(pRecvBuff + pRecvBuffSize, pData, bytesRead);

		dataSourceSize = pRecvBuffSize + bytesRead;
		dataSource = pRecvBuff;
	}

	int bytesReadFromStream;
	while (bytesReadFromStream = BytesFromStream(dataSource, dataSourceSize),
		   LENGTH_FIELD_BYTES + bytesReadFromStream <= dataSourceSize)
	{
		dataSource += LENGTH_FIELD_BYTES;
		dataSourceSize -= LENGTH_FIELD_BYTES;

		TCPSocket::OnRecv(eventHandler, dataSource, bytesReadFromStream);

		dataSource += bytesReadFromStream;
		dataSourceSize -= bytesReadFromStream;
	}

	if (dataSourceSize > 0)
	{
		if (pRecvBuff == NULL)
		{
			pRecvBuff = (uint8_t*)OVR_ALLOC(dataSourceSize);
			memcpy(pRecvBuff, dataSource, dataSourceSize);
		}
		else
		{
			memmove(pRecvBuff, dataSource, dataSourceSize);
		}
	}
	else
	{
		if (pRecvBuff != NULL)
			OVR_FREE(pRecvBuff);

		pRecvBuff = NULL;
	}
	pRecvBuffSize = dataSourceSize;

	recvBuffLock.Unlock();
}

int PacketizedTCPSocket::BytesFromStream(uint8_t* pData, int bytesRead)
{
	if (pData != 0 && bytesRead >= LENGTH_FIELD_BYTES)
	{
		return pData[0] | ((uint32_t)pData[1] << 8) | ((uint32_t)pData[2] << 16) | ((uint32_t)pData[3] << 24);
	}

	return 0;
}


}} // OVR::Net
