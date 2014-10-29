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

-- sdl plugin
minko.plugin.sdl = {}

function minko.plugin.sdl:enable()
	defines { "MINKO_PLUGIN_SDL" }

	minko.plugin.links { "sdl" }

	includedirs {
		minko.plugin.path("sdl") .. "/include",
		minko.plugin.path("sdl") .. "/lib/sdl/include",
	}

	configuration { "windows32" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.plugin.path("sdl") .. "/lib/sdl/lib/windows32" }
		prelinkcommands {
			minko.action.copy(minko.plugin.path("sdl") .. "/lib/sdl/lib/windows32/*.dll")
		}

	configuration { "windows64" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.plugin.path("sdl") .. "/lib/sdl/lib/windows64" }
		prelinkcommands {
			minko.action.copy(minko.plugin.path("sdl") .. "/lib/sdl/lib/windows64/*.dll")
		}

	configuration { "linux32" }
		links { "SDL2" }

	configuration { "linux64" }
		links { "SDL2" }

	configuration { "osx64" }
		links {
			"SDL2",
			-- "CoreFoundation.framework",
			"Carbon.framework",
			-- "AudioToolbox.framework",
			"AudioUnit.framework",
			"CoreAudio.framework",
			"ForceFeedback.framework"
		}
		libdirs { minko.plugin.path("sdl") .. "/lib/sdl/lib/osx64" }

	configuration { "ios" }
		links {
			"SDL2",
			"CoreAudio.framework",
			"AudioToolbox.framework"
		}
		libdirs { minko.plugin.path("sdl") .. "/lib/sdl/lib/ios" }

	configuration { "html5" }
		removeincludedirs { minko.plugin.path("sdl") .. "/lib/sdl/include" }
		includedirs { "SDL" }
		minko.plugin.enable { "webgl" }

	configuration { "android" }
		links { "SDL2" }
		libdirs { minko.plugin.path("sdl") .. "/lib/sdl/lib/android" }
		includedirs { minko.plugin.path("sdl") .. "/lib/sdl/src/core/android" }
		minko.plugin.enable { "android" }

	configuration { "offscreen" }
		minko.plugin.enable { "offscreen" }

end

function minko.plugin.sdl:dist(pluginDistDir)
	configuration { "windows32" }
		os.mkdir(pluginDistDir .. "/lib/sdl/lib/windows32")
		minko.os.copyfiles(minko.plugin.path("sdl") .. "/lib/sdl/lib/windows32", pluginDistDir .. "/lib/sdl/lib/windows32")

	configuration { "windows64" }
		os.mkdir(pluginDistDir .. "/lib/sdl/lib/windows64/lib")
		minko.os.copyfiles(minko.plugin.path("sdl") .. "/lib/sdl/lib/windows64", pluginDistDir .. "/lib/sdl/lib/windows64")
end

newoption {
	trigger			= "with-sdl",
	description		= "Enable the Minko SDL plugin."
}
