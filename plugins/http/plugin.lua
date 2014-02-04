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

-- http plugin
minko.plugin.http = {}

function minko.plugin.http:enable()
	minko.plugin.links { "http" }
	includedirs { minko.sdk.path("plugins/http/include") }
	defines { "MINKO_PLUGIN_HTTP" }
	
	configuration { "win" }
		links { "libcurl"}
		
	configuration { "win", "debug" }
		libdirs { minko.sdk.path("plugins/http/lib/win/libcurl/lib/Debug") }
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/http/lib/win/libcurl/lib/Debug"))
		}
		
	configuration { "win", "release" }
		libdirs { minko.sdk.path("plugins/http/lib/win/libcurl/lib/Debug") }
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/http/lib/win/libcurl/lib/Debug"))
		}
		
	configuration { "linux" }
		links { "curl" }

	configuration { "osx" }
		links { "curl" }
end

function minko.plugin.http:dist(pluginDistDir)
	os.mkdir(pluginDistDir .. "/lib/win/libcurl/lib")
	minko.os.copyfiles(minko.sdk.path("plugins/http/lib/win/libcurl/lib"), pluginDistDir .. "/lib/win/libcurl/lib")
end
