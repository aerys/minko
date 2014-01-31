if not MINKO_HOME then
	if os.getenv('MINKO_HOME') then
		MINKO_HOME = os.getenv('MINKO_HOME');
	else
		print(color.fg.red .. 'You must define the environment variable MINKO_HOME.' .. color.reset)
		os.exit(1)
	end
end

if not os.isfile(MINKO_HOME .. '/sdk.lua') then
	print(color.fg.red ..'MINKO_HOME does not point to a valid Minko SDK.' .. color.reset)
	os.exit(1)
end

print('Minko SDK home directory: ' .. MINKO_HOME)

package.path = MINKO_HOME .. "/modules/?/?.lua;".. package.path

require 'emscripten'
require 'android'
require 'vs2013ctp'

local insert = require 'insert'

insert.insert(premake.tools.gcc, 'cxxflags.system', {
	linux = { "-MMD", "-MP", "-std=c++11" },
	macosx = { "-MMD", "-MP", "-std=c++11" },
	emscripten = { "-MMD", "-MP", "-std=c++11" }
})

insert.insert(premake.tools.gcc, 'tools.linux', {
	ld = MINKO_HOME .. '/tools/lin/bin/g++-ld.sh',
	cxx = MINKO_HOME .. '/tools/lin/bin/g++-ld.sh'
})

insert.insert(premake.tools.clang, 'cxxflags.system', {
	macosx = { "-MMD", "-MP", "-std=c++11", "-stdlib=libc++" }
})

insert.insert(premake.tools.clang, 'ldflags.system', {
	macosx = { "-stdlib=libc++" }
})

configuration { "osx" }
	system "macosx"

configuration { "html5" }
	system "emscripten"

configuration { "android"}
	system "android"

configuration { "cc=gcc"}
	toolset "gcc"

configuration { "cc=clang"}
	toolset "clang"

configuration {}

-- print(table.inspect(premake.tools.clang))

-- distributable SDK
MINKO_SDK_DIST = true

-- import build system utilities
dofile(MINKO_HOME .. '/tools/all/lib/minko.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.sdk.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.os.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.path.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.plugin.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.vs.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.project.lua')

-- options
newoption {
	trigger	= 'no-stencil',
	description = 'Disable stencil buffer support.'
}
if _OPTIONS['no-stencil'] then
	defines { 'MINKO_NO_STENCIL' }
	print('Stencil buffer support is disabled (--no-stencil)')
end

newoption {
	trigger	= 'no-glsl-struct',
	description = 'Disable GLSL struct support.'
}
if _OPTIONS['no-glsl-struct'] then
	defines { 'MINKO_NO_GLSL_STRUCT' }
	print('GLSL structs support is disabled (--no-glsl-struct)')
end

