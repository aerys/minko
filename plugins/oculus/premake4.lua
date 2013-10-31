newoption {
	trigger			= "with-oculus",
	description		= "Enable the Minko Oculus VR plugin."
}

minko.project.library "plugin-oculus"
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c" }
	includedirs { "src" }
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
