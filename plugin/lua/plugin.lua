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

-- lua plugin
minko.plugin.lua = {}

function minko.plugin.lua:enable()
	defines {
		"MINKO_PLUGIN_LUA",
		"LUA_USE_POSIX"
	}

	includedirs {
		minko.plugin.path("lua") .. "/include",
		minko.plugin.path("lua") .. "/lib/lua/include",
		minko.plugin.path("lua") .. "/lib/LuaGlue/include",
	}

	configuration { "not StaticLib" }
		minko.plugin.links { "lua" }

		prelinkcommands {
			minko.action.copy(minko.plugin.path("lua") .. "/asset"),
		}
end

function minko.plugin.lua:dist(pluginDistDir)
	os.mkdir(pluginDistDir .. '/lib/lua/include')
	minko.os.copyfiles(
		minko.plugin.path("lua") .. "/lib/lua/include",
		pluginDistDir .. '/lib/lua/include'
	)
	os.mkdir(pluginDistDir .. '/lib/LuaGlue/include')
	minko.os.copyfiles(
		minko.plugin.path("lua") .. "/lib/LuaGlue/include",
		pluginDistDir .. '/lib/LuaGlue/include'
	)
end

newoption {
	trigger		= "with-lua",
	description	= "Enable the Minko Lua plugin."
}
