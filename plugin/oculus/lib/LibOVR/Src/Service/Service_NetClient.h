/************************************************************************************

Filename    :   Service_NetClient.h
Content     :   Client for service interface
Created     :   June 12, 2014
Authors     :   Michael Antonov, Kevin Jenkins, Chris Taylor

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

#ifndef OVR_Service_NetClient_h
#define OVR_Service_NetClient_h

#include "../Net/OVR_NetworkTypes.h"
#include "Service_NetSessionCommon.h"
#include "../Kernel/OVR_System.h"
#include "../OVR_CAPI.h"
#include "../Util/Util_Render_Stereo.h"

namespace OVR { namespace Service {

using namespace OVR::Net;


//-------------------------------------------------------------------------------------
// NetClient

class NetClient : public NetSessionCommon,
                  public Net::Plugins::NetworkPlugin,
                  public SystemSingletonBase<NetClient>
{
    OVR_DECLARE_SINGLETON(NetClient);
    virtual void OnThreadDestroy();

    // Status
    bool          LatencyTesterAvailable;
    int           HMDCount;
    bool          EdgeTriggeredHMDCount;

    virtual void OnReceive(Net::ReceivePayload* pPayload, Net::ListenerReceiveResult* lrrOut);
    virtual void OnDisconnected(Net::Connection* conn);
    virtual void OnConnected(Net::Connection* conn);

    virtual int  Run();

public:
    bool         Connect(bool blocking);
    bool         IsConnected(bool attemptReconnect, bool blockOnReconnect);
    void         Disconnect();

    void         GetLocalProtocolVersion(int& major, int& minor, int& patch);
    // This function may fail if it is not connected
    bool         GetRemoteProtocolVersion(int& major, int& minor, int& patch);

    void         SetLastError(String str);

public:
    // Persistent key-value storage
    const char*  GetStringValue(VirtualHmdId hmd, const char* key, const char* default_val);
    bool         GetBoolValue(VirtualHmdId hmd, const char* key, bool default_val);
    int          GetIntValue(VirtualHmdId hmd, const char* key, int default_val);
    double       GetNumberValue(VirtualHmdId hmd, const char* key, double default_val);
    int          GetNumberValues(VirtualHmdId hmd, const char* key, double* values, int num_vals);

    bool         SetStringValue(VirtualHmdId hmd, const char* key, const char* val);
    bool         SetBoolValue(VirtualHmdId hmd, const char* key, bool val);
    bool         SetIntValue(VirtualHmdId hmd, const char* key, int val);
    bool         SetNumberValue(VirtualHmdId hmd, const char* key, double val);
    bool         SetNumberValues(VirtualHmdId hmd, const char* key, const double* vals, int num_vals);

    bool         GetDriverMode(bool& driverInstalled, bool& compatMode, bool& hideDK1Mode);
    bool         SetDriverMode(bool compatMode, bool hideDK1Mode);

	int          Hmd_Detect();
	bool         Hmd_Create(int index, HMDNetworkInfo* netInfo);
	void         Hmd_Release(VirtualHmdId hmd);

	// Last string is cached locally.
	const char*  Hmd_GetLastError(VirtualHmdId hmd);

	// TBD: Replace with a function to return internal, original HMDInfo?

	// Fills in description about HMD; this is the same as filled in by ovrHmd_Create.
	// The actual descriptor is a par
	bool         Hmd_GetHmdInfo(VirtualHmdId hmd, HMDInfo* hmdInfo);

	//-------------------------------------------------------------------------------------
	unsigned int Hmd_GetEnabledCaps(VirtualHmdId hmd);
	// Returns new caps after modification
	unsigned int Hmd_SetEnabledCaps(VirtualHmdId hmd, unsigned int hmdCaps);

    // Updates driver render target
    bool         Hmd_AttachToWindow(VirtualHmdId hmd, void* hWindow);

	//-------------------------------------------------------------------------------------
	// *** Tracking Setup

	bool         Hmd_ConfigureTracking(VirtualHmdId hmd, unsigned supportedCaps, unsigned requiredCaps);	
	void         Hmd_ResetTracking(VirtualHmdId hmd);

	// TBD: Camera frames
    bool         LatencyUtil_ProcessInputs(double startTestSeconds, unsigned char rgbColorOut[3]);
    const char*  LatencyUtil_GetResultsString();

    bool         ShutdownServer();

protected:
    String       Hmd_GetLastError_Str;
    String       LatencyUtil_GetResultsString_Str;
    String       ProfileGetValue1_Str, ProfileGetValue3_Str;

protected:
    //// Push Notifications:

    void registerRPC();

    ObserverScope<Net::Plugins::RPCSlot> InitialServerStateScope;
    void InitialServerState_1(BitStream* userData, ReceivePayload* pPayload);

    ObserverScope<Net::Plugins::RPCSlot> LatencyTesterAvailableScope;
    void LatencyTesterAvailable_1(BitStream* userData, ReceivePayload* pPayload);

    ObserverScope<Net::Plugins::RPCSlot> HMDCountUpdateScope;
    void HMDCountUpdate_1(BitStream* userData, ReceivePayload* pPayload);
};


}} // namespace OVR::Service

#endif // OVR_Service_NetClient_h
