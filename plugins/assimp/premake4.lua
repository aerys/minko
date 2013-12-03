newoption {
	trigger = "with-assimp",
	description = "Enable the Minko ASSIMP plugin."
}

minko.project.library "plugin-assimp"
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c" }
	includedirs {
		"include",
		"src"
	}
	
	defines { "DEBUG_SKINNING" }
	
	-- assimp lib
	files {
		"lib/assimp/code/**.c",
		"lib/assimp/code/**.h",
		"lib/assimp/code/**.cpp",
		"lib/assimp/code/**.hpp",
		"lib/assimp/contrib/**.c",
		"lib/assimp/contrib/**.cc",
		"lib/assimp/contrib/**.h",
		"lib/assimp/contrib/**.cpp",
		"lib/assimp/contrib/**.hpp",
	}
	includedirs {
		"lib/assimp/code",
		"lib/assimp/code/BoostWorkaround",
		"lib/assimp/include",
		"lib/assimp/contrib/clipper",
		"lib/assimp/contrib/ConvertUTF",
		"lib/assimp/contrib/irrXML",
		"lib/assimp/contrib/poly2tri",
		"lib/assimp/contrib/unzip",
		"lib/assimp/contrib/zlib"
	}
	defines {
		"ASSIMP_BUILD_BOOST_WORKAROUND",
		"_CRT_SECURE_NO_WARNINGS",
		"_SCL_SECURE_NO_WARNINGS"
	}

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
