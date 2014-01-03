newoption {
	trigger		= "with-lua",
	description	= "Enable the Minko Lua plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.cpp" }
	includedirs { "src", "include" }
	
	-- luaglue
	includedirs { "lib/LuaGlue/include" }

	-- lua
	files { "lib/lua/src/**.c", "lib/lua/include/**.h" }
	includedirs { "lib/lua/include" }
	excludes { "lib/lua/src/luac.c" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	configuration { "debug" }
		defines { "LUA_USE_APICHECK" }
	configuration { "not windows" }
		defines { "LUA_USE_POSIX" }
	