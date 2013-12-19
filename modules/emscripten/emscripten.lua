--
-- Create an emscripten namespace to isolate the additions
--

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

	local gcc = premake.tools.gcc

	gcc.tools.emscripten = {
		cc = MINKO_HOME .. '/tools/lin/bin/emcc.sh',
		cxx = MINKO_HOME .. '/tools/lin/bin/em++.sh',
		ar = MINKO_HOME .. '/tools/lin/bin/emar.sh'
	}

	gcc.cppflags.system.emscripten = { '-MMD', '-DEMSCRIPTEN' }
