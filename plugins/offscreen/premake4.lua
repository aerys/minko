newoption {
	trigger			= "with-offscreen",
	description		= "Enable the Minko Offscreen plugin."
}

minko.project.library "minko-offscreen"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs { "src", "lib/osmesa/include" }
