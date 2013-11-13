newoption {
	trigger = "with-assimp",
	description = "Enable the Minko ASSIMP plugin."
}

minko.project.library "plugin-assimp"
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
