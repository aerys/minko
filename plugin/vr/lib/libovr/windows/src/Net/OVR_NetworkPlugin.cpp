/************************************************************************************

Filename    :   OVR_NetworkPlugin.cpp
Content     :   Base class for an extension to the network objects.
Created     :   June 10, 2014
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

#include "OVR_NetworkPlugin.h"

namespace OVR { namespace Net { namespace Plugins {


//-----------------------------------------------------------------------------
// Plugin identifier to assign next

//static uint8_t pluginIdNext = 0;


//-----------------------------------------------------------------------------
// NetworkPlugin

NetworkPlugin::NetworkPlugin()
{
	pSession = 0;
	//PluginId = pluginIdNext++;
}

NetworkPlugin::~NetworkPlugin()
{
}

void NetworkPlugin::OnAddedToSession(Session* _pSession)
{
	if (pSession != 0)
	{
		pSession->RemoveSessionListener(this);
	}

	pSession = _pSession;
}

void NetworkPlugin::OnRemovedFromSession(Session* _pSession)
{
	OVR_UNUSED(_pSession);
	OVR_ASSERT(_pSession == pSession);

	pSession = 0;
}


}}} // OVR::Net::Plugins
