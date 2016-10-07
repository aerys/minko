if not MINKO_HOME then
	if os.getenv('MINKO_HOME') then
		MINKO_HOME = path.translate(os.getenv('MINKO_HOME'), '/')
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
require 'vs2015ctp'
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
local plugins = {
	['lua'] = false,
	['angle'] = false,
	['zlib'] = true,
	['assimp'] = true,
	['debug'] = true,
	['devil'] = true,
	['bullet'] = true,
	['fx'] = true,
	['html-overlay'] = true,
	['http-loader'] = true,
	['http-worker'] = true,
	['jpeg'] = true,
	['leap'] = false,
	['nodejs-worker'] = false,
	['offscreen'] = false,
	['particles'] = false,
	['png'] = true,
	['sdl'] = true,
	['sensors'] = true,
	['serializer'] = true,
	['video-camera'] = true,
	['vr'] = true,
	['websocket'] = true
}

for plugin, enabled in pairs(plugins) do
	minko.plugin.include(MINKO_HOME .. '/plugin/' .. plugin, enabled)
end
