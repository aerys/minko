newoption {
	trigger			= "with-leap",
	description		= "Enable the Minko Leap plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c", "include/**.hpp" }
	includedirs { "include", "src" }

	removeplatforms { "linux", "html5", "ios", "android" }

	-- windows
	configuration { "windows" }
		includedirs { "lib/include" }