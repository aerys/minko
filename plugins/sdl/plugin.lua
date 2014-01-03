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

function minko.plugin.sdl:import()

end

function minko.plugin.sdl:enable()
	minko.plugin.links { "sdl" }
	defines { "MINKO_PLUGIN_SDL" }
	includedirs { minko.sdk.path("plugins/sdl/include") }

	configuration { "win" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/win/SDL/lib") }
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/sdl/lib/win/SDL/lib"))
		}
		
	configuration { "linux" }
		links { "SDL2" }

	configuration { "osx" }
		links { "SDL2.framework" }

	configuration { "html5" }
		defines { "HAVE_M_PI" }
end

function minko.plugin.sdl:dist(pluginDistDir)
	os.mkdir(pluginDistDir .. "/lib/win/SDL/lib")
	minko.os.copyfiles(minko.sdk.path("plugins/sdl/lib/win/SDL/lib"), pluginDistDir .. "/lib/win/SDL/lib")
end
