/************************************************************************************

Filename    :   OVR_Win32_ShimFunctions.h
Content     :   Client-side shim callbacks for usermode/rt hooks
Created     :   May 6, 2014
Authors     :   Dean Beeler

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

*************************************************************************************/

#ifndef OVR_Win32_ShimFunctions_h
#define OVR_Win32_ShimFunctions_h

#include "OVR_Win32_Display.h"

namespace OVR {

struct Win32ShimInfo;

namespace Win32 {  


class DisplayShim
{
public:

public:
	static DisplayShim& GetInstance()
	{
		static DisplayShim instance;
		return instance;
	}

	bool Initialize( bool inCompatibility );
	bool Shutdown();

	bool Update( Win32ShimInfo* shimInfo );

	void* GetDX11SwapChain();

	ULONG   ChildUid;
	int		ExpectedWidth;
	int		ExpectedHeight;
	int		Rotation;
	HWND    hWindow;
	bool	UseMirroring;
	bool	Active;

private:

	DisplayShim();

	virtual ~DisplayShim();

	DisplayShim(DisplayShim const&);    // Don't Implement
	void operator=(DisplayShim const&); // Don't implement

};


}} // namespace OVR::Win32

#endif
