newoption {
	trigger		= "with-particles",
	description	= "Enable the Minko particles plugin."
}

minko.project.library "plugin-particles"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs { "src" }
