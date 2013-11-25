solution "minko"
	configurations { "debug", "release" }

	dofile('sdk.lua')

	-- examples
	include 'examples/sponza'
	include 'examples/cube'
	include 'examples/light'
	include 'examples/stencil'
	include 'examples/effect-config'
	include 'examples/raycasting'
	--include 'examples/cube-offscreen'

	-- tests
	if _ACTION ~= "vs2010" and _OPTIONS["platform"] ~= "emscripten" then
		include 'tests'
	end
