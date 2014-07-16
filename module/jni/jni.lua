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

api.addAllowed('system', { 'android' })
api.addAllowed("architecture", { "armv5te" })

local ANDROID

if os.getenv('ANDROID') then
	ANDROID = os.getenv('ANDROID');
elseif os.getenv('ANDROID_HOME') then
	ANDROID = os.getenv('ANDROID_HOME');
else
	print(color.fg.yellow .. 'You must define the environment variable ANDROID to be able to target Android.' .. color.reset)
	do return end
end

if not os.isfile(ANDROID .. "/tools/android") then
	error(color.fg.red .. 'Cannot find SDK tools for Android. Make sure ANDROID points to a correct Android SDK directory.' .. color.reset)
end

if not os.isfile(ANDROID .. "/ndk") then
	error(color.fg.red .. 'Cannot find NDK tools for Android. Please install NDK in "' .. ANDROID .. '/ndk".' .. color.reset)
end
NDK_HOME = os.getenv('NDK_HOME')

local TOOLCHAIN = "arm-linux-androideabi"
-- local TOOLCHAIN = "i686-linux-android"

table.inject(premake.tools.gcc, 'tools.android', {
	cc			= ANDROID .. '/ndk' .. '/bin/' .. TOOLCHAIN .. '-gcc',
	cxx			= MINKO_HOME .. '/tool/lin/script/g++.sh ' .. ANDROID .. '/ndk' .. '/bin/' .. TOOLCHAIN .. '-g++',
	ar			= ANDROID .. '/ndk' .. '/bin/' .. TOOLCHAIN .. '-ar',
	ld			= ANDROID .. '/ndk' .. '/bin/' .. TOOLCHAIN .. '-ld',
	ranlib		= ANDROID .. '/ndk' .. '/bin/' .. TOOLCHAIN .. '-ranlib',
	strip		= ANDROID .. '/ndk' .. '/bin/' .. TOOLCHAIN .. '-strip',
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

if not os.isfile(premake.tools.gcc.tools.android.cc) then
	error(color.fg.red ..'Cannot find GCC for Android. Make sure NDK_HOME points to a correct Android NDK directory.' .. color.reset)
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
