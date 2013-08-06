solution "minko"
   configurations { "debug", "release" }
   
-- disable the glsl-optimizer when building for emscripten to avoid linkage issues
if _OPTIONS["platform"] == "emscripten" then
	_OPTIONS["no-glsl-optimizer"] = "true"
end
   
-- examples
--include 'examples/sponza'
--include 'examples/envmap'
--include 'examples/cube'
include 'examples/effect-editor'

-- core framework
include 'framework'

-- plugins
include 'plugins/jpeg'
include 'plugins/png'
if _OPTIONS["platform"] == "emscripten" then
	include 'plugins/webgl'
end
--include 'plugins/mk'
--include 'plugins/bullet'
--include 'plugins/particles'

-- support for new actions
dofile('tools/all/lib/library.lua')
dofile('tools/all/lib/platform.lua')
dofile('tools/all/lib/vs.lua')
