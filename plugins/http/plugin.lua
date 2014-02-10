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
	
	configuration { "windows32 or windows64" }
		links { "libcurl"}
		
	configuration { "windows32 or windows64", "debug" }
		libdirs { minko.sdk.path("plugins/http/lib/win/libcurl/lib/Debug") }
		postbuildcommands {
			minko.action.copy(minko.plugin.path("http") .. "/lib/win/libcurl/lib/Debug/*.dll")
		}
		
	configuration { "windows32 or windows64", "release" }
		libdirs { minko.sdk.path("plugins/http/lib/win/libcurl/lib/Release") }
		postbuildcommands {
			minko.action.copy(minko.plugin.path("http") .. "/lib/win/libcurl/lib/Release/*.dll")
		}
		
	configuration { "linux32 or linux64" }
		links { "curl" }

	configuration { "osx64" }
		links { "curl" }
end

function minko.plugin.http:dist(pluginDistDir)
	os.mkdir(pluginDistDir .. "/lib/win/libcurl/lib")
	minko.os.copyfiles(minko.sdk.path("plugins/http/lib/win/libcurl/lib"), pluginDistDir .. "/lib/win/libcurl/lib")
end
