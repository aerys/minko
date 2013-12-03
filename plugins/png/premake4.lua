newoption {
	trigger			= "with-png",
	description		= "Enable the Minko PNG plugin."
}

minko.project.library "plugin-png"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"include",
		"src",
		"lib/lodepng/src"
	}
	defines
	{
		--"LODEPNG_NO_COMPILE_ENCODER"
		--"LODEPNG_NO_COMPILE_DISK"
		"_CRT_SECURE_NO_WARNINGS"
	}

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
