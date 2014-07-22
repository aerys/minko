premake.extensions.emscripten = {}

local emscripten = premake.extensions.emscripten
local project = premake.project
local api = premake.api
local make = premake.make
local cpp = premake.make.cpp
local project = premake.project
local config = premake.config
local fileconfig = premake.fileconfig

api.addAllowed("system", { "emscripten" })

if os.getenv('EMSCRIPTEN') then
	EMSCRIPTEN = os.getenv('EMSCRIPTEN');
elseif os.getenv('EMSCRIPTEN_HOME') then
	EMSCRIPTEN = os.getenv('EMSCRIPTEN_HOME');
else
	print(color.fg.yellow .. 'You must define the environment variable EMSCRIPTEN to be able to target HTML5.' .. color.reset)
	EMSCRIPTEN = ''
end

if os.is('linux') then
	table.inject(premake.tools.gcc, 'tools.emscripten', {
		cc = MINKO_HOME .. '/tool/lin/script/emcc.sh',
		cxx = MINKO_HOME .. '/tool/lin/script/em++.sh',
		ar = MINKO_HOME .. '/tool/lin/script/emar.sh'
	})
	table.inject(premake.tools.clang, 'tools.emscripten', {
		cc = MINKO_HOME .. '/tool/lin/script/emcc.sh',
		cxx = MINKO_HOME .. '/tool/lin/script/em++.sh',
		ar = MINKO_HOME .. '/tool/lin/script/emar.sh'
	})
elseif os.is('macosx') then
	table.inject(premake.tools.gcc, 'tools.emscripten', {
		cc = MINKO_HOME .. '/tool/mac/script/emcc.sh',
		cxx = MINKO_HOME .. '/tool/mac/script/em++.sh',
		ar = MINKO_HOME .. '/tool/mac/script/emar.sh'
	})
	table.inject(premake.tools.clang, 'tools.emscripten', {
		cc = MINKO_HOME .. '/tool/mac/script/emcc.sh',
		cxx = MINKO_HOME .. '/tool/mac/script/em++.sh',
		ar = MINKO_HOME .. '/tool/mac/script/emar.sh'
	})
elseif os.is('windows') then
	table.inject(premake.tools.gcc, 'tools.emscripten', {
		cc = '"' .. EMSCRIPTEN .. '\\emcc.bat"',
		cxx = 'call "%MINKO_HOME%\\tool\\win\\script\\em++.bat"',
		ar = '"' .. EMSCRIPTEN .. '\\emar.bat"'
	})
	table.inject(premake.tools.clang, 'tools.emscripten', {
		cc = '"' .. EMSCRIPTEN .. '\\emcc.bat"',
		cxx = 'call "%MINKO_HOME%\\tool\\win\\script\\em++.bat"',
		ar = '"' .. EMSCRIPTEN .. '\\emar.bat"'
	})
end

table.inject(premake.tools.gcc, 'cppflags.system.emscripten', {
	"-MMD", "-MP",
	"-DEMSCRIPTEN",
	"-Wno-warn-absolute-paths"
})

table.inject(premake.tools.clang, 'cppflags.system.emscripten', {
	"-MMD", "-MP",
	"-DEMSCRIPTEN",
	"-Wno-warn-absolute-paths"
})

table.inject(premake.tools.gcc, 'cxxflags.system.emscripten', {
	'"-std=c++11"',
})

table.inject(premake.tools.clang, 'cxxflags.system.emscripten', {
	'"-std=c++11"',
})
