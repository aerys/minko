minko.project.application "example-cube-offscreen"
	kind "ConsoleApp"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	includedirs { "src" }

	minko.plugin.enable("webgl");
	minko.plugin.enable("png");

	-- linux
	configuration { "linux" }
		links { "OSMesa" }
