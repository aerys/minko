--[[
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]--

-- vr plugin
minko.plugin.vr = {}

-- Note: if you see errors like this one:
-- /System/Library/Frameworks/IOKit.framework/Headers/usb/USB.h:754:23: error: too many #pragma options align=reset
-- please replace all occurences of #pragma options align=reset with:
-- #if defined(__GNUC__)
-- #pragma pack()
-- #else
-- #pragma options align=reset
-- #endif

-- Note for linux compilation:
-- The 'libudev' and 'libxinerama' libraries must be prealably installed.

function minko.plugin.vr:enable()

    defines { "MINKO_PLUGIN_VR" }
    minko.plugin.links { "vr" }

    includedirs {
        minko.plugin.path("vr") .. "/include"
    }

    prelinkcommands {
        minko.action.copy(path.join(minko.plugin.path("vr"), "asset"))
    }

    minko.package.assetdirs {
        path.join(minko.plugin.path("vr"), "asset")
    }

    configuration { "android or ios or html5" }
        minko.plugin.enable { "sensors" }

    configuration { "windows32 or windows64" }
        links { "winmm", "setupapi", "ws2_32" }

    configuration { "linux32 or linux64" }
        links { "udev", "Xinerama", "X11", "pthread", "rt", "Xrandr" }

    configuration { "html5" }
        minko.package.assets {
            ['**'] = { 'embed' }
        }
end

newoption {
    trigger     = "with-vr",
    description = "Enable the Minko VR plugin."
}
