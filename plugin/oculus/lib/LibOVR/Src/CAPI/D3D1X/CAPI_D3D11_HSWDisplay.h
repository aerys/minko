/************************************************************************************

Filename    :   CAPI_D3D11_HSWDisplay.h
Content     :   Implements Health and Safety Warning system.
Created     :   July 7, 2014
Authors     :   Paul Pedriana

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

#ifndef OVR_CAPI_D3D11_HSWDisplay_h
#define OVR_CAPI_D3D11_HSWDisplay_h

#if !defined(OVR_D3D_VERSION) || ((OVR_D3D_VERSION != 10) && (OVR_D3D_VERSION != 11))
    #error This header expects OVR_D3D_VERSION to be defined, to 10 or 11.
#endif

// Due to the similarities between DX10 and DX11, there is a shared implementation of the headers and source
// which is differentiated only by the OVR_D3D_VERSION define. This define causes D3D_NS (D3D namespace) to 
// be defined to either D3D10 or D3D11, as well as other similar effects. 
#include "CAPI_D3D1X_HSWDisplay.h"


#endif // OVR_CAPI_D3D11_HSWDisplay_h

