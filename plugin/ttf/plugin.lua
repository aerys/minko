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

-- ttf plugin
minko.plugin.ttf = {}

function minko.plugin.ttf:enable()
    defines { "MINKO_PLUGIN_TTF" }

    includedirs { minko.plugin.path("ttf") .. "/include" }

    minko.plugin.links { "ttf" }

    configuration { "windows32", "ConsoleApp or WindowedApp" }
        libdirs { minko.plugin.path("ttf") .. "/lib/freetype/lib/windows32" }
        prelinkcommands {
            minko.action.copy(minko.plugin.path("ttf") .. "/lib/freetype/lib/windows32/*.dll")
        }

    configuration { "windows64", "ConsoleApp or WindowedApp" }
        libdirs { minko.plugin.path("ttf") .. "/lib/freetype/lib/windows64" }
        prelinkcommands {
            minko.action.copy(minko.plugin.path("ttf") .. "/lib/freetype/lib/windows64/*.dll")
        }

    configuration { "windows" }
        defines { "MINKO_PLUGIN_TTF_FREETYPE" }
        links { "freetype28" }

    configuration { "linux" }
        defines { "MINKO_PLUGIN_TTF_FREETYPE" }
        links { "freetype" }

    configuration { "vs*" }
        linkoptions {
            "/NODEFAULTLIB:msvcrt.lib"
        }
end
