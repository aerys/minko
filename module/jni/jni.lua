require 'ext'

premake.extensions.jni = {}

local jni = premake.extensions.jni
local project = premake.project
local api = premake.api
local make = premake.make
local cpp = premake.make.cpp
local project = premake.project
local config = premake.config
local fileconfig = premake.fileconfig

api.addAllowed("system", { "android" })
api.addAllowed("architecture", { "armv5te" })

newoption {
   trigger     = "toolchain",
   description = "Android NDK toolchain"
}

if _ACTION ~= "gmake" then
	do return end
end

if not _OPTIONS["toolchain"] then
   _OPTIONS["toolchain"] = "arm-linux-androideabi-4.9"
end

local ANDROID_HOME = os.getenv('ANDROID_HOME') or os.getenv('ANDROID');

if not ANDROID_HOME then
	print(color.fg.yellow .. 'You must define the environment variable ANDROID_HOME to be able to target Android.' .. color.reset)
	do return end
end

if not os.isfile(ANDROID_HOME .. "/tools/lib/sdk-common.jar") then
	error(color.fg.red .. 'Cannot find SDK tools for Android. Make sure ANDROID_HOME points to a correct Android SDK directory ' ..
		' (missing \'' .. ANDROID_HOME .. '/tools/lib/sdk-common.jar\')' .. color.reset)
end

-- if we try to build Android on Windows without Cygwin
if os.is("windows") and os.getenv('OSTYPE') == nil then
	print(color.fg.yellow .. 'To build for Android on Windows, you have to use Cygwin. ' ..
		'Please check that you exported OSTYPE environment variable.' .. color.reset)
	do return end
end

if not os.isdir(ANDROID_HOME .. "/toolchains") then
	error(color.fg.red .. 'Cannot find NDK tools for Android. Please extract NDK in ${ANDROID_HOME}/ndk/android-ndk-<version>" and run `install_jni.sh` or `install_jni.bat`.' .. color.reset)
end

-- writing toolchain name in a fake symlink to avoid actual symlinks on Windows (requiring privileges)
local NDK_HOME = ANDROID_HOME .. "/toolchains/" .. _OPTIONS["toolchain"]


if not os.isdir(NDK_HOME) then
	error(color.fg.red .. 'NDK is not correctly installed: ' .. NDK_HOME .. color.reset)
end

local function find(binary)
	local extension = os.is("windows") and '.exe' or ''
	matches = os.matchfiles(path.join(NDK_HOME, 'bin', '*-' .. binary .. extension))
	if #matches == 0 then
		error(color.fg.red .. binary .. ' not found in NDK' .. color.reset)
	else
		return matches[1]
	end
end

table.inject(premake.tools.gcc, 'tools.android', {
	cc			= find('gcc'),
	cxx			= 'CXX="' .. find('g++') .. '" ' .. path.cygpath(MINKO_HOME) .. '/module/gcc/g++.sh',
	ar			= find('ar'),
	ld			= find('ld'),
	strip		= find('strip')
})

table.inject(premake.tools.gcc, 'cppflags.system.android', {
	"-MMD", "-MP"
})

table.inject(premake.tools.gcc, 'cxxflags.system.android', {
	"-std=c++11"
})

table.inject(premake.tools.gcc, 'ldflags.system.android', {
	"-Wl,--fix-cortex-a8",
})

if not os.capture("which ant") then
	error(color.fg.red ..'Cannot find Ant. Make sure "ant" is available in your path.' .. color.reset)
end
