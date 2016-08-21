--[[
Copyright (c) 2016 Aerys

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

-- websocket plugin
minko.plugin.websocket = {}

function minko.plugin.websocket:enable()
	minko.plugin.links { "websocket" }

	includedirs { minko.plugin.path("websocket") .. "/include" }

	defines { "MINKO_PLUGIN_WEBSOCKET" }

	configuration { "linux" }
		links {
			"ssl",
			"crypto"
		}

	configuration { "android" }
		libdirs {
			minko.plugin.path("websocket") .. "/lib/openssl/lib/android"
		}
		links {
			"ssl",
			"crypto"
		}

	configuration { "windows32" }
		links {
			minko.plugin.path("websocket") .. "/lib/openssl/lib/windows32/ssleay32",
			minko.plugin.path("websocket") .. "/lib/openssl/lib/windows32/libeay32"
		}
		prelinkcommands {
			minko.action.copy(minko.plugin.path("websocket") .. "/lib/openssl/lib/windows32/ssleay32.dll"),
			minko.action.copy(minko.plugin.path("websocket") .. "/lib/openssl/lib/windows32/libeay32.dll")
		}

	configuration { "windows64" }
		links {
			minko.plugin.path("websocket") .. "/lib/openssl/lib/windows64/ssleay32",
			minko.plugin.path("websocket") .. "/lib/openssl/lib/windows64/libeay32"
		}
		prelinkcommands {
			minko.action.copy(minko.plugin.path("websocket") .. "/lib/openssl/lib/windows64/ssleay32.dll"),
			minko.action.copy(minko.plugin.path("websocket") .. "/lib/openssl/lib/windows64/libeay32.dll")
		}
end
