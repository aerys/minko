/************************************************************************************

Filename    :   CAPI_DistortionRenderer.cpp
Content     :   Combines all of the rendering state associated with the HMD
Created     :   February 2, 2014
Authors     :   Michael Antonov

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

#include "CAPI_DistortionRenderer.h"

#if defined (OVR_OS_WIN32)

// TBD: Move to separate config file that handles back-ends.
#define OVR_D3D_VERSION 11
#include "D3D1X/CAPI_D3D1X_DistortionRenderer.h"
#undef OVR_D3D_VERSION

#define OVR_D3D_VERSION 10
#include "D3D1X/CAPI_D3D1X_DistortionRenderer.h"
#undef OVR_D3D_VERSION

#define OVR_D3D_VERSION 9
#include "D3D9/CAPI_D3D9_DistortionRenderer.h"
#undef OVR_D3D_VERSION

#endif

#include "GL/CAPI_GL_DistortionRenderer.h"

namespace OVR { namespace CAPI {

//-------------------------------------------------------------------------------------
// ***** DistortionRenderer

// TBD: Move to separate config file that handles back-ends.

DistortionRenderer::CreateFunc DistortionRenderer::APICreateRegistry[ovrRenderAPI_Count] =
{
    0, // None
    &GL::DistortionRenderer::Create,
    0, // Android_GLES
#if defined (OVR_OS_WIN32)
    &D3D9::DistortionRenderer::Create,
    &D3D10::DistortionRenderer::Create,
    &D3D11::DistortionRenderer::Create
#else
    0,
    0,
    0
#endif
};

void DistortionRenderer::SetLatencyTestColor(unsigned char* color)
{
    if(color)
    {
        LatencyTestDrawColor[0] = color[0];
        LatencyTestDrawColor[1] = color[1];
        LatencyTestDrawColor[2] = color[2];
    }

    LatencyTestActive = color != NULL;
}

void DistortionRenderer::SetLatencyTest2Color(unsigned char* color)
{
    if(color)
    {
        LatencyTest2DrawColor[0] = color[0];
        LatencyTest2DrawColor[1] = color[1];
        LatencyTest2DrawColor[2] = color[2];
    }

    LatencyTest2Active = color != NULL;
}

void DistortionRenderer::GetOverdriveScales(float& outRiseScale, float& outFallScale)
{
    outRiseScale = 0.1f;
    outFallScale = 0.05f;	// falling issues are hardly visible
}

double DistortionRenderer::WaitTillTime(double absTime)
{
    double initialTime = ovr_GetTimeInSeconds();
    if (initialTime >= absTime)
        return 0.0;

    double newTime = initialTime;

    while (newTime < absTime)
    {
// TODO: Needs further testing before enabling it on all Windows configs
#if 0 //def OVR_OS_WIN32
        double remainingWaitTime = absTime - newTime;

        // don't yield if <2ms
        if(remainingWaitTime > 0.002)
        {
            // round down wait time to closest 1 ms
            int roundedWaitTime = (remainingWaitTime * 1000);

            waitableTimerInterval.QuadPart = -10000LL; // 10000 * 100 ns = 1 ms
            waitableTimerInterval.QuadPart *= roundedWaitTime;

            SetWaitableTimer(timer, &waitableTimerInterval, 0, NULL, NULL, TRUE);
            DWORD waitResult = WaitForSingleObject(timer, roundedWaitTime + 3);   // give 3 ms extra time
            OVR_UNUSED(waitResult);

#ifdef OVR_BUILD_DEBUG
            double sleptTime = ovr_GetTimeInSeconds() - newTime;
            // Make sure we didn't sleep too long and it is reliable, otherwise we might miss v-sync causing a stutter
            if (sleptTime > (roundedWaitTime + 2) * 0.001) 
            {
                OVR_DEBUG_LOG_TEXT(
                    ("[DistortionRenderer::WaitTillTime] Sleep interval too long: %f\n", sleptTime));
            }
            else
            {
                OVR_ASSERT(WAIT_OBJECT_0 == waitResult);
            }
#endif
        }
        else
#endif
        {
            for (int j = 0; j < 5; j++)
                OVR_PROCESSOR_PAUSE();
        }

        newTime = ovr_GetTimeInSeconds();
    }

    // How long we waited
    return newTime - initialTime;
}

}} // namespace OVR::CAPI

