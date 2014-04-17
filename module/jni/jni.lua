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

NDK_HOME = os.getenv('NDK_HOME')

if NDK_HOME then
	table.inject(premake.tools.gcc, 'tools.android', {
		cc			= os.capture(NDK_HOME .. '/ndk-which gcc'),
		cxx			= os.capture(NDK_HOME .. '/ndk-which g++'),
		ar			= os.capture(NDK_HOME .. '/ndk-which ar'),
		ld			= os.capture(NDK_HOME .. '/ndk-which ld'),
		ranlib		= os.capture(NDK_HOME .. '/ndk-which ranlib'),
		strip		= os.capture(NDK_HOME .. '/ndk-which strip'),
	})

	table.inject(premake.tools.gcc, 'cppflags.system.android', {
		"-MMD", "-MP", "-std=c++11",
		"--sysroot=" .. NDK_HOME .. "/platforms/android-9/arch-arm",
		"-I" .. NDK_HOME .. "/sources/cxx-stl/gnu-libstdc++/4.8/include/",
		"-I" .. NDK_HOME .. "/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/include"
	})

	table.inject(premake.tools.gcc, 'ldflags.system.android', {
		"-Wl,--fix-cortex-a8",
		"--sysroot=" .. NDK_HOME .. "/platforms/android-9/arch-arm"
	})

	if not os.isfile(premake.tools.gcc.tools.android.cc) then
		error(color.fg.red ..'Cannot find GCC for Android. Make sure NDK_HOME points to a correct Android NDK directory.' .. color.reset)
	end
else
	print(color.fg.yellow .. 'You must define the environment variable NDK_HOME to be able to target Android.' .. color.reset)
end
