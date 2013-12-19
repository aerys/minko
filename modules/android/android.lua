--
-- Create an android namespace to isolate the additions
--

	premake.extensions.android = {}

	local android = premake.extensions.android
	local project = premake.project
	local api = premake.api

	api.addAllowed('system', { 'android' })

	local make = premake.make
	local cpp = premake.make.cpp
	local project = premake.project
	local config = premake.config
	local fileconfig = premake.fileconfig

	local gcc = premake.tools.gcc

	gcc.tools.android = {
		cc			= ANDROID_HOME .. '/bin/arm-linux-androideabi-gcc',
		cxx			= ANDROID_HOME .. '/bin/arm-linux-androideabi-g++',
		ar			= ANDROID_HOME .. '/bin/arm-linux-androideabi-ar',
		ld			= ANDROID_HOME .. '/bin/arm-linux-androideabi-ld',
		ranlib		= ANDROID_HOME .. '/bin/arm-linux-androideabi-ranlib',
		strip		= ANDROID_HOME .. '/bin/arm-linux-androideabi-strip',
	}

	gcc.cppflags.system.android = { }
	gcc.cxxflags.system.android = { '--sysroot=' .. ANDROID_HOME .. '/sysroot/', '-fno-rtti', '-fno-exceptions' }
	gcc.ldflags.system.android = { '-L' .. ANDROID_HOME .. '/sysroot/usr/lib' }

	-- targetprefix "lib"
	-- targetextension ".so"
	-- linkoptions { "-llog", "-lGLESv1_CM", "-lz", "-s", "-shared" }
