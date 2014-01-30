newoption {
	trigger		= "with-lua",
	description	= "Enable the Minko Lua plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.cpp", "include/**.hpp" }
	includedirs { "src", "include" }
	
	-- luaglue
	includedirs { "lib/LuaGlue/include" }
	files { "lib/LuaGlue/include/**.h" }

	-- lua
	files { "lib/lua/src/**.c", "lib/lua/include/**.h" }
	includedirs { "lib/lua/include" }
	excludes { "lib/lua/src/luac.c" }

	configuration { "debug" }
		defines { "LUA_USE_APICHECK" }

	configuration { "html5" }
		defines { "LUA_USE_POSIX" }

	configuration { "linux" }
		defines { "LUA_USE_POSIX" }

	configuration { "win" }
		defines { "LUA_USE_WIN" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration { "osx" }
		defines { "LUA_USE_MACOSX" }
