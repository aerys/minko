dofile('sdk.lua')

minko.project.solution "minko"

	-- plugins
	include 'plugins/jpeg'
	include 'plugins/png'
	include 'plugins/mk'
	include 'plugins/bullet'
	include 'plugins/particles'
	include 'plugins/sdl'
	include 'plugins/angle'
	if _OPTIONS["platform"] == "emscripten" then
		include 'plugins/webgl'
	end

	-- examples
	include 'examples/sponza'
	include 'examples/cube'
	include 'examples/light'
	include 'examples/stencil'
	include 'examples/effect-config'
	--include 'examples/cube-offscreen'

	-- tests
	if _ACTION ~= "vs2010" and _OPTIONS["platform"] ~= "emscripten" then
		include 'tests'
	end
