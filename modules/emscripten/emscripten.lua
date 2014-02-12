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

if os.getenv('EMSCRIPTEN') then
	EMSCRIPTEN = os.getenv('EMSCRIPTEN');
else
	print(color.fg.yellow .. 'You must define the environment variable EMSCRIPTEN to be able to target HTML5.' .. color.reset)
end

print(color.fg.yellow .. 'Foo bar.' .. color.reset)

local insert = require 'insert'

if os.is('linux') then
	insert.insert(premake.tools.gcc, 'tools.emscripten', {
		cc = MINKO_HOME .. '/tools/lin/scripts/emcc.sh',
		cxx = MINKO_HOME .. '/tools/lin/scripts/em++.sh',
		ar = MINKO_HOME .. '/tools/lin/scripts/emar.sh'
	})
elseif os.is('macosx') then
	insert.insert(premake.tools.gcc, 'tools.emscripten', {
		cc = MINKO_HOME .. '/tools/mac/scripts/emcc.sh',
		cxx = MINKO_HOME .. '/tools/mac/scripts/em++.sh',
		ar = MINKO_HOME .. '/tools/mac/scripts/emar.sh'
	})
elseif os.is('windows') then
	insert.insert(premake.tools.gcc, 'tools.emscripten', {
		cc = '"' .. EMSCRIPTEN .. '\\emcc.bat"',
		cxx = '"' .. EMSCRIPTEN .. '\\em++.bat"',
		ar = '"' .. EMSCRIPTEN .. '\\emar.bat"'
	})
end

insert.insert(premake.tools.gcc, 'cppflags.system.emscripten', {
	"-DEMSCRIPTEN",
	"-Wno-warn-absolute-paths"
})
