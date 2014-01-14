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

	configuration { "windows", "x32" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/sdl/windows32/lib") }
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/sdl/lib/sdl/windows32/lib"))
		}

	configuration { "windows", "x64" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/sdl/windows64/lib") }
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/sdl/lib/sdl/windows64/lib"))
		}
		
	configuration { "linux", "x32" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/sdl/linux32/lib") }

	configuration { "linux", "x64" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/sdl/linux64/lib") }

	configuration { "macosx" }
		links { "SDL2.framework" }
		libdirs { minko.sdk.path("plugins/sdl/lib/sdl/osx64/lib") }

	configuration { "html5" }
		defines { "HAVE_M_PI" }


end

function minko.plugin.sdl:dist(pluginDistDir)
	print("plugin-dist " .. pluginDistDir)
	

	configuration { "windows", "x32" }
		os.mkdir(pluginDistDir .. "/lib/sdl/windows32/lib")
		minko.os.copyfiles(minko.sdk.path("plugins/sdl/lib/sdl/windows32/lib"), pluginDistDir .. "/lib/sdl/windows32/lib")

	configuration { "windows", "x64" }
		os.mkdir(pluginDistDir .. "/lib/sdl/windows64/lib")
		minko.os.copyfiles(minko.sdk.path("plugins/sdl/lib/sdl/windows64/lib"), pluginDistDir .. "/lib/sdl/windows64/lib")
end
