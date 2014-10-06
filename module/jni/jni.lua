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

local ANDROID
local TOOLCHAIN = "arm-linux-androideabi"

-- If we try to build Android on Windows without Cygwin
if os.is("windows")  and os.getenv('OSTYPE') == nil then
	return
end

if os.getenv('ANDROID_HOME') then
	ANDROID = os.getenv('ANDROID_HOME');
else
	error(color.fg.red .. 'You must define the environment variable ANDROID_HOME to be able to target Android.' .. color.reset)
end

if not os.isfile(ANDROID .. "/tools/android") and not os.isfile(ANDROID .. "/tools/android.bat") then
	error(color.fg.red .. 'Cannot find SDK tools for Android. Make sure ANDROID points to a correct Android SDK directory.' .. 
		'(Missing file: \'' .. ANDROID .. '/tools/android\' or \'' .. ANDROID .. '/tools/android.bat\')' .. color.reset)
end

if not os.isdir(ANDROID .. "/toolchains") then
	error(color.fg.red .. 'Cannot find NDK tools for Android. Please install NDK in "' .. ANDROID .. '/toolchains" and run `install_jni.sh` or `install_jni.bat`.' .. color.reset)
end

-- writing toolchain name in a fake symlink to avoid actual symlinks on Windows (requiring privileges)
local NDK_HOME = ANDROID .. "/toolchains/default"
local extension = ''

if not os.isfile(NDK_HOME) then
	error(color.fg.red .. 'Installed NDK is not correctly installed: ' .. NDK_HOME .. color.reset)
end

if os.is("windows") then
	NDK_HOME = os.capture('cygpath -u "' .. NDK_HOME .. '"')
	extension = '.exe'
end

-- local matches = os.matchdirs(NDK_HOME .. "/*abi")

-- local TOOLCHAIN

-- for i, basedir in ipairs(matches) do
-- 	TOOLCHAIN = path.getbasename(basedir)
-- 	break
-- end

-- if not TOOLCHAIN then
-- 	error(color.fg.red .. 'Installed NDK is not correctly installed: ' .. NDK_HOME .. color.reset)
-- end

table.inject(premake.tools.gcc, 'tools.android', {
	cc			= NDK_HOME .. '/bin/' .. TOOLCHAIN .. '-gcc' .. extension,
	cxx			= MINKO_HOME .. '/tool/lin/script/g++.sh ' .. NDK_HOME .. '/bin/' .. TOOLCHAIN .. '-g++' .. extension,
	ar			= NDK_HOME .. '/bin/' .. TOOLCHAIN .. '-ar' .. extension,
	ld			= NDK_HOME .. '/bin/' .. TOOLCHAIN .. '-ld' .. extension,
	ranlib		= NDK_HOME .. '/bin/' .. TOOLCHAIN .. '-ranlib' .. extension,
	strip		= NDK_HOME .. '/bin/' .. TOOLCHAIN .. '-strip' .. extension,
})

table.inject(premake.tools.gcc, 'cppflags.system.android', {
	"-MMD", "-MP",
	-- "--sysroot=" .. NDK_HOME .. "/sysroot",
	-- "-I" .. NDK_HOME .. "/sources/cxx-stl/gnu-libstdc++/4.8/include/",
	-- "-I" .. NDK_HOME .. "/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/include",
	-- "-L" .. NDK_HOME .. "/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/"
})

table.inject(premake.tools.gcc, 'cxxflags.system.android', {
	"-std=c++11"
})

table.inject(premake.tools.gcc, 'ldflags.system.android', {
	-- "--sysroot=" .. NDK_HOME .. "/platforms/android-9/arch-arm",
	"-Wl,--fix-cortex-a8",
	-- "-L" .. NDK_HOME .. "/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/"
})

local file = premake.tools.gcc.tools.android.cc

if os.is('windows') then
	file = os.capture('cygpath -aw "' .. file .. '"')
end

if not os.isfile(file) then
	error(color.fg.red ..'Cannot find GCC for Android. Make sure ANDROID contains NDK.' ..
		' (Missing file: ' .. file .. ')' .. color.reset)
end

if not os.capture("which ant") then
	error(color.fg.red ..'Cannot find Ant. Make sure "ant" is available in your path.' .. color.reset)
end

-- -- Specify android ABIs
-- api.register {
-- 	name = "abis",
-- 	scope = "config",
-- 	kind = "string",
-- 	list = "true",
-- 	allowed = {
-- 		"all",
-- 		"armeabi",
-- 		"armeabi-v7a",
-- 		"mips",
-- 		"x86"
-- 	}
-- }

-- -- Specify android STL support
-- api.register {
-- 	name = "stl",
-- 	scope = "config",
-- 	kind = "string",
-- 	allowed = {
-- 		"gabi++_static",
-- 		"gabi++_shared",
-- 		"gnustl_static",
-- 		"gnustl_shared",
-- 		"stlport_static",
-- 		"stlport_shared",
-- 		"system"
-- 	}
-- }

-- -- Specify android package name
-- api.register {
-- 	name = "packagename",
-- 	scope = "project",
-- 	kind = "string"
-- }

-- -- Specify android package version
-- api.register {
-- 	name = "packageversion",
-- 	scope = "project",
-- 	kind = "integer"
-- }

-- -- Specify android activity name
-- api.register {
-- 	name = "activity",
-- 	scope = "project",
-- 	kind = "string"
-- }

-- -- Specify android activity base class
-- api.register {
-- 	name = "baseactivity",
-- 	scope = "project",
-- 	kind = "string"
-- }

-- -- Specify android activity base package
-- api.register {
-- 	name = "basepackagename",
-- 	scope = "project",
-- 	kind = "string"
-- }

-- -- Specify applicaton permissions
-- api.register {
-- 	name = "permissions",
-- 	scope = "config",
-- 	kind = "string",
-- 	list = true
-- }
