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

-- android plugin
minko.plugin.android = {}

function minko.plugin.android:enable()
	configuration { "android" }
		defines { "MINKO_PLUGIN_ANDROID" }

		minko.plugin.links { "android" }
		includedirs { minko.plugin.path("android") .. "/include" }
		-- undefined allow us to keep the existence of a global 
		-- variable even if it's not explicitly used on Android
		linkoptions { "-Wl,--undefined=gAndroidLogInitializing"}

		if kind() ~= "StaticLib" then
			postbuildcommands {
				-- 'echo ' .. abis(),
				'bash ' .. minko.plugin.path("android") .. '/script/build_android.sh ${TARGET} || ' .. minko.action.fail()
			}
		end
end
