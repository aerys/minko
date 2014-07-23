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

package.path = MINKO_HOME .. "/module/?/?.lua;".. package.path

print('Minko SDK home directory: ' .. MINKO_HOME)

require 'ext'
require 'minko'
require 'emscripten'
require 'jni'
require 'vs2013ctp'
require 'gcc'
require 'clang'
require 'xcode'

configurations {
	"debug",
	"release"
}

minko.platform.platforms {
	"linux32",
	"linux64",
	"windows32",
	"windows64",
	"osx64",
	"html5",
	"ios",
	"android",
}

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
	toolset "clang"

configuration { "html5" }
	system "emscripten"

configuration { "android"}
	system "android"

configuration { "cc=gcc"}
	toolset "gcc"

configuration { "cc=clang", "not html5" }
	toolset "clang"

configuration {}

-- distributable SDK
MINKO_SDK_DIST = true

-- make plugins visible from an external project
local plugins = os.matchdirs(MINKO_HOME .. '/plugin/*')

for _, plugin in ipairs(plugins) do
	minko.plugin.include(plugin)
end

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

