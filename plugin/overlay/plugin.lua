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
	
	minko.plugin.enable("lua")

	configuration { "windows32 or windows64" }
		links { "libcef" }
		prelinkcommands {
			minko.action.copy(minko.plugin.path("overlay") .. "/asset"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/resource/*"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/win/dll/*.dll")
		}

	configuration { "windows32 or windows64", "debug"}
		libdirs { minko.plugin.path("overlay") .. "/lib/win/debug" }

	configuration { "windows32 or windows64", "release"}
		libdirs { minko.plugin.path("overlay") .. "/lib/win/release" }

	configuration { "linux32 or linux64"}
		buildoptions {
			"-pthread"
		}
		linkoptions {
			"-Wl,-rpath,."
		}
		links { "cef" }
		libdirs { minko.plugin.path("overlay") .. "/lib/lin/" }
		prelinkcommands {
			minko.action.copy(minko.plugin.path("overlay") .. "/asset"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/resource/locales"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/resource/cef.pak"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/resource/devtools_resources.pak"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/lin/libcef.so"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/lin/libffmpegsumo.so"),
			minko.action.copy(minko.plugin.path("overlay") .. "/lib/lin/chrome-sandbox")
		}
end

function minko.plugin.overlay:dist(pluginDistDir)
	configuration { "windows32 or windows64" }
		os.mkdir(pluginDistDir .. "/lib/win/")
		minko.os.copyfiles(minko.plugin.path("overlay") .. "/lib/win/", pluginDistDir .. "/lib/win/")

	configuration { "linux32 or linux64" }
		os.mkdir(pluginDistDir .. "/lib/lin/")
		os.mkdir(pluginDistDir .. "/lib/resource/")
		minko.os.copyfiles(minko.plugin.path("overlay") .. "/lib/lin/", pluginDistDir .. "/lib/lin/")
		minko.os.copyfiles(minko.plugin.path("overlay") .. "/lib/resource/", pluginDistDir .. "/lib/resource/")
end

newoption {
	trigger			= "with-overlay",
	description		= "Enable the Minko HTML Overlay plugin."
}
