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

-- http worker plugin
minko.plugin["http-worker"] = {}

minko.plugin["http-worker"].enable = function (self)
--function minko.plugin.["http-worker"]:enable()
	includedirs { minko.plugin.path("http-worker") .. "/include" }
	defines { "MINKO_PLUGIN_HTTP_WORKER" }

	defines { "CURL_STATICLIB" }

	configuration { "not html5" }
		minko.plugin.links { "http-worker" }

	configuration { "windows32 or windows64" }
		links { "libcurl" }

	configuration { "windows32" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/windows32" }
		postbuildcommands {
			minko.action.copy(minko.plugin.path("http-worker") .. "/lib/curl/lib/windows32/*.dll")
		}

	configuration { "windows64" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/windows64" }
		postbuildcommands {
			minko.action.copy(minko.plugin.path("http-worker") .. "/lib/curl/lib/windows64/*.dll")
		}

	configuration { "linux32 or linux64" }
		links { "curl" }

	configuration { "osx64" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/osx64/release" }
		links { "curl", "minko-plugin-zlib", "Security.framework", "LDAP.framework" }

	configuration { "osx64", "debug" }
		libdirs { minko.plugin.path("zlib") .. "/bin/osx64/debug" }

	configuration { "osx64", "release" }
		libdirs { minko.plugin.path("zlib") .. "/bin/osx64/release" }

	configuration { "ios" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/ios/release" }
		links { "curl", "minko-plugin-zlib", "Security.framework" }

	configuration { "ios", "debug" }
		libdirs { minko.plugin.path("zlib") .. "/bin/ios/debug" }

	configuration { "ios", "release" }
		libdirs { minko.plugin.path("zlib") .. "/bin/ios/release" }

	configuration { "android" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/android/release" }
		links { "curl", "minko-plugin-zlib" }
		
	configuration { "android", "debug" }
		libdirs { minko.plugin.path("zlib") .. "/bin/android/debug" }

	configuration { "android", "release" }
		libdirs { minko.plugin.path("zlib") .. "/bin/android/release" }
end

--function minko.worker.http:dist(workerDistDir)
--	os.mkdir(workerDistDir .. "/lib/curl/lib")
--	minko.os.copyfiles(minko.worker.path("http") .. "/lib/curl/lib", workerDistDir .. "/lib/curl/lib")
--end

newoption {
	trigger		= "with-http-worker",
	description	= "Enable the Minko HTTP Worker plugin."
}
