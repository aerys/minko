newoption {
	trigger		= "with-jpeg",
	description	= "Enable the Minko JPEG plugin."
}

minko.project.library "plugin-jpeg"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"include",
		"src",
		"lib/jpeg-compressor/src"
	}
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
