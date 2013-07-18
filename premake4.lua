solution "minko"
   configurations { "debug", "release" }

-- examples
include 'examples'

-- core framework
include 'framework'

-- plugins
include 'plugins/jpeg'
include 'plugins/png'
include 'plugins/webgl'

-- support for new actions
dofile('tools/all/lib/library.lua')
dofile('tools/all/lib/vs.lua')
dofile('tools/all/lib/platform.lua')
