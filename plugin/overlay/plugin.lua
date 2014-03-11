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

-- overlay (html5 overlay) plugin
minko.plugin.overlay = {}

function minko.plugin.overlay:enable()
	minko.plugin.links { "overlay" }
	defines { "MINKO_PLUGIN_OVERLAY" }
	includedirs {
		minko.plugin.path("overlay") .. "/include"
	}

	configuration { "windows32 or windows64", "debug" }
		links { "libcef_dll_wrapper", "libcef" }
		libdirs { minko.plugin.path("overlay") .. "/lib/win/cef3/lib/debug" }
		prelinkcommands {
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/win/cef3/resource"),
			minko.action.copy(minko.plugin.path("overlay") .. "/asset"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/win/cef3/lib/debug/*.dll")
		}

	configuration { "windows32 or windows64", "release" }
		links { "libcef_dll_wrapper", "libcef" }
		libdirs { minko.plugin.path("overlay") .. "/lib/win/cef3/lib/release" }
		prelinkcommands {
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/win/cef3/resource"),
			minko.action.copy(minko.plugin.path("overlay") .. "/asset"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/win/cef3/lib/release/*.dll")
		}
		
	prelinkcommands {
	}
end

function minko.plugin.overlay:dist(pluginDistDir)
	configuration { "windows32 or windows64", "debug" }
		os.mkdir(pluginDistDir .. "/lib/win/cef3/lib/debug")
		minko.os.copyfiles(minko.plugin.path("overlay") .. "/lib/win/cef3/lib/debug", pluginDistDir .. "/lib/win/cef3/lib/debug")
		
	configuration { "windows32 or windows64", "release" }
		os.mkdir(pluginDistDir .. "/lib/win/cef3/lib/release")
		minko.os.copyfiles(minko.plugin.path("overlay") .. "/lib/win/cef3/lib/release", pluginDistDir .. "/lib/win/cef3/lib/release")
end

newoption {
	trigger			= "with-overlay",
	description		= "Enable the Minko HTML Overlay plugin."
}
