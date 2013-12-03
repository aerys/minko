newoption {
	trigger			= "with-offscreen",
	description		= "Enable the Minko Offscreen plugin."
}

minko.project.library "minko-offscreen"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs { "include", "src", "lib/osmesa/include" }
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
