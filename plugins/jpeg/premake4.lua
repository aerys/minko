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
