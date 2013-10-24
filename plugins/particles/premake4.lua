newoption {
	trigger		= "with-particles",
	description	= "Enable the Minko particles plugin."
}

minko.project.library "minko-particles"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs { "src" }

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
	