solution "minko"
   configurations { "debug", "release" }

-- support for new actions
dofile('tools/all/lib/library.lua')
dofile('tools/all/lib/vs.lua')
dofile('tools/all/lib/platform.lua')

-- examples
include 'examples/mk-example'
if _OPTIONS["platform"] ~= "emscripten" then
	include 'examples/cube'
end

-- core framework
include 'framework'

-- plugins
include 'plugins/jpeg'
include 'plugins/png'
if _OPTIONS["platform"] == "emscripten" then
	include 'plugins/webgl'
end
include 'plugins/mk'
include 'plugins/bullet'
include 'plugins/particles'
