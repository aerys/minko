newoption {
	trigger		= "with-lua",
	description	= "Enable the Minko LUA plugin."
}

minko.project.library "plugin-lua"
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c" }
	includedirs { "src" }
	
	-- lua
	files { "lib/lua/src/**.cpp", "lua/src/**.h" }
	includedirs { "lib/lua/src" }
	
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
