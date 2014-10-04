/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_NetworkPlugin.h
Content     :   Base class for an extension to the network objects.
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

#ifndef OVR_NetworkPlugin_h
#define OVR_NetworkPlugin_h

#include "OVR_Session.h"

namespace OVR { namespace Net { namespace Plugins {


//-----------------------------------------------------------------------------
// NetworkPlugin

// NetworkPlugins use Session and SessionListener to provide network functionality
// independent of the transport medium.
// Uses the chain of command design pattern such that plugins can invoke or intercept
// network events via the Session.
class NetworkPlugin : public SessionListener
{
public:
	NetworkPlugin();
	virtual ~NetworkPlugin();

protected:
	virtual void OnAddedToSession(Session* _pSession);
	virtual void OnRemovedFromSession(Session* _pSession);

	Session *pSession;
	//uint8_t PluginId;
};


}}} // OVR::Net::Plugins

#endif
