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

-- http worker
minko.worker.http = {}

function minko.worker.http:enable()
	minko.worker.links { "http" }
	includedirs { minko.worker.path("http") .. "/include" }
	defines { "MINKO_WORKER_HTTP" }

	-- minko.plugin.enable("http")

	defines { "CURL_STATICLIB" }

	configuration { "windows32 or windows64" }
		links { "libcurl" }
		
	configuration { "windows32", "debug" }
		libdirs { minko.worker.path("http") .. "/lib/curl/lib/windows32/debug" }
		postbuildcommands {
			minko.action.copy(minko.worker.path("http") .. "/lib/curl/lib/windows32/debug/*.dll")
		}
		
	configuration { "windows32", "release" }
		libdirs { minko.worker.path("http") .. "/lib/curl/lib/windows32/release" }
		postbuildcommands {
			minko.action.copy(minko.worker.path("http") .. "/lib/curl/lib/windows32/release/*.dll")
		}
		
	configuration { "windows64", "debug" }
		libdirs { minko.worker.path("http") .. "/lib/curl/lib/windows64/debug" }
		postbuildcommands {
			minko.action.copy(minko.worker.path("http") .. "/lib/curl/lib/windows64/debug/*.dll")
		}
		
	configuration { "windows64", "release" }
		libdirs { minko.worker.path("http") .. "/lib/curl/lib/windows64/release" }
		postbuildcommands {
			minko.action.copy(minko.worker.path("http") .. "/lib/curl/lib/windows64/release/*.dll")
		}
		
	configuration { "linux32 or linux64" }
		links { "curl" }

	configuration { "osx64" }
		links { "curl" }
end

function minko.plugin.http:dist(pluginDistDir)
	os.mkdir(pluginDistDir .. "/lib/curl/lib/win")
	minko.os.copyfiles(minko.worker.path("/lib/curl/lib/win"), pluginDistDir .. "/lib/curl/lib/win")
end
