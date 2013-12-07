newoption {
	trigger		= "with-jpeg",
	description	= "Enable the Minko JPEG plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"include",
		"src",
		"lib/jpeg-compressor/src"
	}
