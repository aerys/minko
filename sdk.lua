if not MINKO_HOME then
	if os.getenv('MINKO_HOME') then
		MINKO_HOME = os.getenv('MINKO_HOME');
	else
		error('You must define the environment variable MINKO_HOME.')
	end
end

if not os.isfile(MINKO_HOME .. '/sdk.lua') then
	error('MINKO_HOME does not point to a valid Minko SDK.')
end

package.path = MINKO_HOME .. "/modules/?/?.lua;".. package.path

print('Minko SDK home directory: ' .. MINKO_HOME)

configurations {
	"debug",
	"release"
}

platforms {
	"linux32",
	"linux64",
	"windows32",
	"windows64",
	"osx64",
	"html5",
	-- "ios",
	-- "android"
}

require 'minko'
require 'color'
require 'emscripten'
-- require 'android'
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

configuration { "windows32" }
	system "windows"
	architecture "x32"

configuration { "windows64" }
	system "windows"
	architecture "x64"

configuration { "linux32" }
	system "linux"
	architecture "x32"

configuration { "linux64" }
	system "linux"
	architecture "x64"

configuration { "osx64" }
	system "macosx"

configuration { "html5" }
	system "emscripten"

-- configuration { "android"}
-- 	system "android"

configuration { "cc=gcc"}
	toolset "gcc"

configuration { "cc=clang"}
	toolset "clang"

configuration {}

-- distributable SDK
MINKO_SDK_DIST = true

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
