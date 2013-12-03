solution "minko"
	configurations { "debug", "release" }

	dofile('sdk.lua')
	
	include 'framework'
	
	-- plugins
	include 'plugins/jpeg'
	include 'plugins/png'
	include 'plugins/mk'
	include 'plugins/bullet'
	include 'plugins/particles'
	include 'plugins/sdl'
	include 'plugins/angle'
	include 'plugins/fx'
	include 'plugins/assimp'
	include 'plugins/offscreen'
	if _OPTIONS["platform"] == "emscripten" then
		include 'plugins/webgl'
	end

	-- tests
	if _ACTION ~= "vs2010" and _OPTIONS["platform"] ~= "emscripten" then
		include 'tests'
	end
