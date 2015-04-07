/*******************************************************************************

Filename    :   OVR_Linux_SDKWindow.h
Content     :   SDK generated Linux window.
Created     :   October 1, 2014
Authors     :   James Hughes

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

*******************************************************************************/

#ifndef OVR_Linux_SDKWindow_h
#define OVR_Linux_SDKWindow_h

#include "../OVR_CAPI.h"
#include "../CAPI/GL/CAPI_GL_Util.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xresource.h>
#include <GL/glx.h>

namespace OVR {

enum DistortionRotation
{
    DistRotateNone,
    DistRotateCCW90
};

struct LinuxDeviceScreen
{
    LinuxDeviceScreen() :
        screen(-1),
        crtcid(0),
        rotation(DistRotateNone),
        productCode(-1),
        width(-1),
        height(-1),
        offsetX(-1),
        offsetY(-1)
    {}

    void set(int xScreen, XID xid, DistortionRotation rot, int prodCode,
             int w, int h, int x, int y)
    {
        screen      = xScreen;
        crtcid      = xid;
        rotation    = rot;
        productCode = prodCode;

        width       = w;
        height      = h;
        offsetX     = x;
        offsetY     = y;
    }

    bool isValid()      {return (screen != -1);}

    int                 screen;   ///< X Screen this device occupies.
    XID                 crtcid;   ///< XID uniquely identifying this device on XDisplay.
    DistortionRotation  rotation;
    int                 productCode;

    // Actual width and height of screen.
    int                 width;
    int                 height;

    // Offset if using twinview
    int                 offsetX;
    int                 offsetY;
};

class SDKWindow
{
public:
    
    SDKWindow(const ovrHmd& hmd);
    ~SDKWindow();

    /// Rotation necessary to correctly orient this SDK window.
    DistortionRotation GetDistortionRotation() {return mDeviceScreen.rotation;}

    _XDisplay* GetDisplay()         {return mXDisplay;}
    XVisualInfo* GetVisual()        {return mXVisual;}
    GLXFBConfig GetFBConfig()       {return mFBConfig;}
    Window GetDrawable()            {return mXWindow;}
    bool HasValidWindow()           {return (mXWindow != 0);}

    // If cfg is non-null it is populated with the chosen configuration.
    static XVisualInfo* chooseVisual(_XDisplay* display, int xscreen,
                                     GLXFBConfig* cfg = NULL);

    static LinuxDeviceScreen findDevScreenForHMD(const ovrHmd& hmd);
    static LinuxDeviceScreen findDevScreenForDevID(const char* deviceID);

    static DistortionRotation getRotation(const ovrHmd& hmd);

    // Obtains XVisualInfo for currently bound context. Returns true if a
    // visual was successfully obtained. False otherwise.
    static bool getVisualFromDrawable(GLXDrawable drawable, XVisualInfo* vinfoOut);

    // TODO: Function which uses glXGetConfig to generate an FB config from
    //       a visual. Chooses first FBConfig if multiple matches found.
    //static GLXFBConfig* getFBConfigFromVisual(XVisualInfo* vis);

private:

    /// Constructs SDK window on the given device screen.
    void buildVisualAndWindow(const LinuxDeviceScreen& devScreen);

    // Added m in front of variables so as to not conflict with X names.
    _XDisplay*       mXDisplay;
    int              mXScreen;
    XVisualInfo*     mXVisual;
    XContext         mXUniqueContext;
    Window           mXWindow;
    GLXFBConfig      mFBConfig;

    LinuxDeviceScreen mDeviceScreen;
};


} // namespace OVR

#endif // OVR_Linux_SDKWindow_h
