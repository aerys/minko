solution "minko"
   configurations { "debug", "release" }

-- examples
include 'examples/mk-example'
include 'examples/cube'

-- core framework
include 'framework'

-- plugins
include 'plugins/jpeg'
include 'plugins/png'
include 'plugins/webgl'
include 'plugins/mk'
include 'plugins/bullet'
include 'plugins/particles'

-- support for new actions
dofile('tools/all/lib/library.lua')
dofile('tools/all/lib/vs.lua')
dofile('tools/all/lib/platform.lua')
