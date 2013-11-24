newoption {
	trigger		= "with-lua",
	description	= "Enable the Minko LUA plugin."
}

minko.project.library "plugin-lua"
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.cpp" }
	includedirs { "src" }
	
	-- lua
	files { "lib/lua/src/**.c", "lua/src/**.h" }
	includedirs { "lib/lua/src" }
	excludes { "lib/lua/src/luac.c" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
	-- luaglue
	includedirs { "lib/LuaGlue/include" }
			
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
