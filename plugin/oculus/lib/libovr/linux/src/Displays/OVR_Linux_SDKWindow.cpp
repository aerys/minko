/*******************************************************************************

Filename    :   OVR_Linux_SDKWindow.cpp
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

#include "OVR_Linux_SDKWindow.h"
#include "../Kernel/OVR_Log.h"
#include "../Kernel/OVR_Log.h"
#include "../../../3rdParty/EDID/edid.h"

namespace OVR {

// Forward declarations
static Window       constructWindow(_XDisplay* display, int xscreen,
                                    XVisualInfo* xvisual,
                                    const LinuxDeviceScreen& screen);

static XRRModeInfo* findModeByXID(XRRScreenResources* screen, RRMode xid)
{
  for (int m = 0; m < screen->nmode; ++m)
  {
    XRRModeInfo* mode = &screen->modes[m];
    if (xid == mode->id)
    {
      return mode;
    }
  }
  return NULL;
}

/// Retrieves a list of available device screens on which we can build
/// SDK windows. Returns number of devices found.
///   screens        Array which this function will populate.
///   maxNumScreens  Maximum number of screens to store in screens.
static int getDeviceScreens(LinuxDeviceScreen* screens, int maxNumDevices)
{
    _XDisplay* disp = XOpenDisplay(NULL);
    if (!disp)
    {
        OVR::LogError("[SDKWindow] Unable to open X Display.");
        return 0;
    }

    int numDevices = 0;
    int numScreens = XScreenCount(disp);
    for (int i = 0; i < numScreens; ++i)
    {
        // Screen root is used to detect what video output the crtc is using.
        Window sr                  = XRootWindow(disp, i);
        XRRScreenResources* screen = XRRGetScreenResources(disp, sr);

        for (int ii = 0; ii < screen->ncrtc; ++ii)
        {
            XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(disp, screen, screen->crtcs[ii]);

            if (0 == crtcInfo->noutput)
            {
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            bool foundOutput = false;
            RROutput output = crtcInfo->outputs[0];
            for (int k = 0; k < crtcInfo->noutput; ++k)
            {
                XRROutputInfo* outputInfo =
                    XRRGetOutputInfo(disp, screen, crtcInfo->outputs[k]);
                for (int kk = 0 ; kk < outputInfo->nmode; ++kk)
                {
                    if (outputInfo->modes[kk] == crtcInfo->mode)
                    {
                        output = crtcInfo->outputs[k];
                        foundOutput = true;
                        break;
                    }
                }
                XRRFreeOutputInfo(outputInfo);
                if (foundOutput) { break; }
            }

            if (!foundOutput)
            {
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            XRROutputInfo* outputInfo = XRRGetOutputInfo(disp, screen, output);
            if (RR_Connected != outputInfo->connection)
            {
                XRRFreeOutputInfo(outputInfo);
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            // Read EDID associated with crtc.
            MonitorInfo* mi = read_edid_data(disp, output);
            if (mi == NULL)
            {
                XRRFreeOutputInfo(outputInfo);
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            if (strcmp(mi->manufacturer_code, "OVR") == 0)
            {
                XRRModeInfo* modeInfo = findModeByXID(screen, crtcInfo->mode);

                DistortionRotation desiredRot = DistRotateNone;
                if (mi->product_code == 3)
                {
                    // This is a DK2, we may have to rotate our output.
                    // If we don't have to, we should alert the user that
                    // rotating the display using the DM or graphics
                    // card settings is highly non-optimal.
                    desiredRot = DistRotateCCW90;
                    if (crtcInfo->rotation != RR_Rotate_0)
                    {
                        OVR::LogError("Please do not rotate your rift's screen.");

                        if (crtcInfo->rotation == RR_Rotate_90)
                        {
                            // The user has manually rotated the screen.
                            // So apply no rotation on our end.
                            desiredRot = DistRotateNone;
                        }
                    }
                }
                else
                {
                    if (crtcInfo->rotation != RR_Rotate_0)
                    {
                        OVR::LogError("Please do not rotate your rift's screen.");
                    }
                }

                int width = modeInfo->width;
                int height = modeInfo->height;

                // Swap width / height if display is rotated (shouldn't be on linux).
                if (   crtcInfo->rotation == RR_Rotate_90
                    || crtcInfo->rotation == RR_Rotate_270)
                {
                    width  = modeInfo->height;
                    height = modeInfo->width;
                }

                // Push detected monitor.
                screens[numDevices].set(i, screen->crtcs[ii], desiredRot,
                                        mi->product_code, width, height,
                                        crtcInfo->x, crtcInfo->y);
                ++numDevices;
            }

            delete mi;

            if (numDevices == maxNumDevices)
            {
                XRRFreeOutputInfo(outputInfo);
                XRRFreeCrtcInfo(crtcInfo);
                XRRFreeScreenResources(screen);
                OVR::LogError("[SDKWindow] Maxed out number of devices..");
                XCloseDisplay(disp);
                return numDevices;
            }

            XRRFreeOutputInfo(outputInfo);
            XRRFreeCrtcInfo(crtcInfo);
        }

        XRRFreeScreenResources(screen);
    }
    XCloseDisplay(disp);
    return numDevices;
}


LinuxDeviceScreen SDKWindow::findDevScreenForHMD(const ovrHmd& hmd)
{
    return findDevScreenForDevID(hmd->DisplayDeviceName);
}

LinuxDeviceScreen SDKWindow::findDevScreenForDevID(const char* deviceIDIn)
{
    const int maxNumDevices = 5;
    LinuxDeviceScreen screens[maxNumDevices];
    int numDevices = getDeviceScreens(screens, maxNumDevices);

    if (numDevices > 0)
    {
        // Identify target for SDK window via hmd info.
        for (int i = 0; i < numDevices; ++i)
        {
            LinuxDeviceScreen& screen = screens[i];

            char deviceID[32];
            OVR_sprintf(deviceID, 32, "OVR%04d-%d",
                        screen.productCode, screen.crtcid);

            if (strcmp(deviceIDIn, deviceID) == 0)
            {
                return screen;
            }
        }
    }

    return LinuxDeviceScreen();
}

DistortionRotation SDKWindow::getRotation(const ovrHmd& hmd)
{
    LinuxDeviceScreen screen = findDevScreenForHMD(hmd);
    if (screen.isValid())
    {
        return screen.rotation;
    }
    else
    {
        return DistRotateNone;
    }
}


bool SDKWindow::getVisualFromDrawable(GLXDrawable drawable, XVisualInfo* vinfoOut)
{
    _XDisplay* display = glXGetCurrentDisplay();

    unsigned int value;
    glXQueryDrawable(display, drawable, GLX_FBCONFIG_ID, &value);
    XVisualInfo* chosen = glXGetVisualFromFBConfig(display, reinterpret_cast<GLXFBConfig>(value));
    *vinfoOut = *chosen;
    return true;
}

SDKWindow::SDKWindow(const ovrHmd& hmd) :
    mXDisplay(NULL),
    mXScreen(-1),
    mXVisual(NULL),
    mXUniqueContext(-1),
    mXWindow(0),
    mFBConfig(NULL)
{
    OVR_UNUSED(hmd);
}

SDKWindow::~SDKWindow()
{
    if (mXWindow)
    {
        XDeleteContext(mXDisplay, mXWindow, mXUniqueContext);
        XUnmapWindow(mXDisplay, mXWindow);
        XDestroyWindow(mXDisplay, mXWindow);
        mXWindow = static_cast<Window>(0);
    }

    if (mXDisplay != NULL)
    {
        XCloseDisplay(mXDisplay);
    }
}

void SDKWindow::buildVisualAndWindow(const LinuxDeviceScreen& devScreen)
{
    mXDisplay       = XOpenDisplay(NULL);
    mXUniqueContext = XUniqueContext();
    mXScreen        = devScreen.screen;

    mXVisual = chooseVisual(mXDisplay, mXScreen, &mFBConfig);
    if (mXVisual != NULL)
    {
        mXWindow = constructWindow(mXDisplay, mXScreen, mXVisual, devScreen);
        mDeviceScreen = devScreen;
    }
}

// Used in chooseVisual. May need to expose this to the end use so they can
// choose an appropriate framebuffer configuration.
struct FBConfig
{
  FBConfig() :
      redBits(8),
      greenBits(8),
      blueBits(8),
      alphaBits(8),
      depthBits(8),
      stencilBits(-1),
      doubleBuffer(true),
      auxBuffers(-1)
  {}

  int  redBits;
  int  greenBits;
  int  blueBits;
  int  alphaBits;
  int  depthBits;
  int  stencilBits;
  bool doubleBuffer;
  int  auxBuffers;

  GLXFBConfig xcfg;
};

static int fbCalcContrib(int desired, int current)
{
    int diff = desired - current;
    if (current != -1) { return diff * diff; }
    else               { return 0; }
}

/// Selects ideal visual for the given screen. Returns NULL on error.
XVisualInfo* SDKWindow::chooseVisual(_XDisplay* display, int xscreen,
                                     GLXFBConfig* cfg)
{
    // Select visual.
    int nativeCount = 0;
    GLXFBConfig* nativeConfigs =
        glXGetFBConfigs(display, xscreen, &nativeCount);
    if (!nativeCount)
    {
        OVR::LogError("[SDKWindow] No valid frame buffer configurations found.");
        return NULL;
    }

    FBConfig* usables = static_cast<FBConfig*>(calloc(nativeCount, sizeof(FBConfig)));
    int numUsables = 0;

    for (int i = 0; i < nativeCount; ++i)
    {
        GLXFBConfig native = nativeConfigs[i];
        FBConfig* usable   = &usables[numUsables];
        int v              = 0;

        // Only frame buffer configcs with attached visuals.
        glXGetFBConfigAttrib(display, native, GLX_VISUAL_ID, &v);
        if (!v) { continue; }

        // Only RGBA frame buffers.
        glXGetFBConfigAttrib(display, native, GLX_RENDER_TYPE, &v);
        if (!(v & GLX_RGBA_BIT)) { continue; }

        glXGetFBConfigAttrib(display, native, GLX_DRAWABLE_TYPE, &v);
        if (!(v & GLX_WINDOW_BIT)) { continue; }

        glXGetFBConfigAttrib(display, native, GLX_DEPTH_SIZE,   &usable->depthBits);
        glXGetFBConfigAttrib(display, native, GLX_STENCIL_SIZE, &usable->stencilBits);

        glXGetFBConfigAttrib(display, native, GLX_RED_SIZE,     &usable->redBits);
        glXGetFBConfigAttrib(display, native, GLX_GREEN_SIZE,   &usable->greenBits);
        glXGetFBConfigAttrib(display, native, GLX_BLUE_SIZE,    &usable->blueBits);
        glXGetFBConfigAttrib(display, native, GLX_ALPHA_SIZE,   &usable->alphaBits);

        glXGetFBConfigAttrib(display, native, GLX_ALPHA_SIZE,   &usable->auxBuffers);

        glXGetFBConfigAttrib(display, native, GLX_DOUBLEBUFFER, &v);
        usable->doubleBuffer = v ? true : false;

        usable->xcfg = native;

        ++numUsables;
    }

    // We really want std::numeric_limits<int>::max() instead of hardcoded vals.
    const int MostMissing = 100;
    int leastMissing      = MostMissing;
    int leastBias         = MostMissing;

    const FBConfig* closest = NULL;
    
    // Desired is currently the default config built by constructor.
    FBConfig desired;

    for (int i = 0; i < numUsables; ++i)
    {
        const FBConfig* cur = &usables[i];

        if (desired.doubleBuffer != cur->doubleBuffer) { continue; }

        int missing = 0;
        if (desired.alphaBits > 0 && cur->alphaBits == 0) { ++missing; }
        if (desired.depthBits > 0 && cur->depthBits == 0) { ++missing; }
        if (desired.stencilBits > 0 && cur->stencilBits == 0) { ++missing; }
        if (desired.redBits > 0 && desired.redBits != cur->redBits) { ++missing; }
        if (desired.greenBits > 0 && desired.greenBits != cur->greenBits) { ++missing; }
        if (desired.blueBits > 0 && desired.blueBits != cur->blueBits) { ++missing; }

        int bias = fbCalcContrib(desired.redBits,     cur->redBits)
                 + fbCalcContrib(desired.greenBits,   cur->greenBits)
                 + fbCalcContrib(desired.blueBits,    cur->blueBits)
                 + fbCalcContrib(desired.alphaBits,   cur->alphaBits)
                 + fbCalcContrib(desired.depthBits,   cur->depthBits)
                 + fbCalcContrib(desired.stencilBits, cur->stencilBits);

        if (missing < leastMissing)
        {
            closest = cur;
        }
        else if (missing == leastMissing)
        {
            // Now select against squared differences.
            if (bias < leastBias)
            {
                closest = cur;
            }
        }

        if (closest == cur)
        {
            leastMissing = missing;
            leastBias    = bias;
        }
    }

    if (closest == NULL)
    {
        OVR::LogError("[SDKWindow] Failed to select appropriate frame buffer.");
        XFree(nativeConfigs);
        free(usables);
        return NULL;
    }

    OVR_DEBUG_LOG(("[C] Chosen framebuffer config:"));
    OVR_DEBUG_LOG(("[C]   Double buffer:  %d", closest->doubleBuffer));
    OVR_DEBUG_LOG(("[C]   RGBA bits:     (%d,%d,%d,%d)",
                   closest->redBits, closest->greenBits, closest->blueBits,
                   closest->alphaBits));
    OVR_DEBUG_LOG(("[C]   Depth bits:     %d", closest->depthBits));

    XVisualInfo* viOut = glXGetVisualFromFBConfig(display, closest->xcfg);
    if (cfg != NULL)
    {
        *cfg = closest->xcfg;
    }

    XFree(nativeConfigs);
    free(usables);

    return viOut;
}

static int gXLastError = -1;
static int handleXError(_XDisplay* display, XErrorEvent* event)
{
  OVR_UNUSED(display);
  gXLastError = event->error_code;
  return 0;
}

static void obtainXErrorHandler()
{
  gXLastError = Success;
  XSetErrorHandler(handleXError);
}

static void releaseXErrorHandler(_XDisplay* display)
{
  XSync(display, False);
  XSetErrorHandler(NULL);
}

// Returns 0 on error, otherwise a valid X window is returned.
static Window constructWindow(_XDisplay* xDisp, int xScreen,
                              XVisualInfo* xVisual,
                              const LinuxDeviceScreen& devScreen)
{
    XSetWindowAttributes wa;

    Window root       = XRootWindow(xDisp, xScreen);
    Window xWindowOut = 0;

    // Create Window
    {
        Colormap xWinColorMapOut = XCreateColormap(
            xDisp, root, xVisual->visual, AllocNone);
        unsigned long wamask = CWBorderPixel | CWColormap | CWEventMask;

        wa.colormap = xWinColorMapOut;
        wa.border_pixel = 0;
        wa.event_mask = StructureNotifyMask  | ExposureMask    | FocusChangeMask
                      | VisibilityChangeMask | EnterWindowMask | LeaveWindowMask
                      | PropertyChangeMask;

        obtainXErrorHandler();

        xWindowOut = XCreateWindow(xDisp, root,
                                   0, 0,
                                   devScreen.width, devScreen.height,
                                   0,
                                   xVisual->depth,
                                   InputOutput,
                                   xVisual->visual,
                                   wamask,
                                   &wa);

        releaseXErrorHandler(xDisp);

        if (!xWindowOut)
        {
            OVR::LogError("[SDKWindow] Failed to create SDK window.");
            return 0;
        }

        XFreeColormap(xDisp, xWinColorMapOut);
    }

    // OVERRIDE REDIRECT.
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(xDisp, xWindowOut,
                            CWOverrideRedirect, &attributes);

    // Show the window (do this in full screen or windowed).
    XMapRaised(xDisp, xWindowOut);
    XFlush(xDisp);

    // Position ourselves manually since there should be no WM managing us.
    XRaiseWindow(xDisp, xWindowOut);
    XMoveWindow(xDisp, xWindowOut, devScreen.offsetX, devScreen.offsetY);
    XResizeWindow(xDisp, xWindowOut, devScreen.width, devScreen.height);

    XFlush(xDisp);

    // WM Backup in case there still exists a WM managing us...
    Atom NET_WM_BYPASS_COMPOSITOR = 
        XInternAtom(xDisp, "_NET_WM_BYPASS_COMPOSITOR", False);
    Atom NET_WM_STATE =
        XInternAtom(xDisp, "_NET_WM_STATE", False);
    Atom NET_WM_STATE_FULLSCREEN =
        XInternAtom(xDisp, "_NET_WM_STATE_FULLSCREEN", False);
    Atom NET_ACTIVE_WINDOW =
        XInternAtom(xDisp, "_NET_ACTIVE_WINDOW", False);

    // Bypass compositor if we are under a compositing WM.
    // Just in case a WM ignores our override_redirect.
    if (NET_WM_BYPASS_COMPOSITOR)
    {
        const unsigned long bypass = 1;
        XChangeProperty(xDisp, xWindowOut,
                        NET_WM_BYPASS_COMPOSITOR,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char*)&bypass, 1);
    }

    if (NET_WM_STATE && NET_WM_STATE_FULLSCREEN)
    {
        // BACKUP: If we are still managed by a WM we want fullscreen.
        const int EWMH_STATE_ADD    = 1;

        if (NET_ACTIVE_WINDOW)
        {
            XEvent event;
            memset(&event, 0, sizeof(event));

            event.type = ClientMessage;
            event.xclient.window = xWindowOut;
            event.xclient.format = 32;
            event.xclient.message_type = NET_ACTIVE_WINDOW;
            event.xclient.data.l[0] = 1;
            event.xclient.data.l[1] = 0;

            XSendEvent(xDisp, root, False,
                       SubstructureNotifyMask | SubstructureRedirectMask,
                       &event);
        }

        XEvent event;
        memset(&event, 0, sizeof(event));

        event.type = ClientMessage;
        event.xclient.window = xWindowOut;
        event.xclient.format = 32;
        event.xclient.message_type = NET_WM_STATE;
        event.xclient.data.l[0] = EWMH_STATE_ADD;
        event.xclient.data.l[1] = NET_WM_STATE_FULLSCREEN;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 1;

        XSendEvent(xDisp, root, False,
                   SubstructureNotifyMask | SubstructureRedirectMask,
                   &event);
    }

    return xWindowOut;
}

} // namespace OVR

