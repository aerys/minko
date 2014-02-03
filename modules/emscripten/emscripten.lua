premake.extensions.emscripten = {}

local emscripten = premake.extensions.emscripten
local project = premake.project
local api = premake.api

api.addAllowed("system", { "emscripten" })

local make = premake.make
local cpp = premake.make.cpp
local project = premake.project
local config = premake.config
local fileconfig = premake.fileconfig

local insert = require 'insert'

insert.insert(premake.tools.gcc, 'tools.emscripten', {
	cc = MINKO_HOME .. '/tools/lin/bin/emcc.sh',
	cxx = MINKO_HOME .. '/tools/lin/bin/em++.sh',
	ar = MINKO_HOME .. '/tools/lin/bin/emar.sh'
})

insert.insert(premake.tools.gcc, 'cppflags.system.emscripten', {
	'-DEMSCRIPTEN'
})

if os.getenv('EMSCRIPTEN') then
	EMSCRIPTEN = os.getenv('EMSCRIPTEN');
end
