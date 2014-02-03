local insert = require 'insert'
local inspect = require 'inspect'

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

insert.insert(premake.tools.gcc, 'tools.android', {
	cc			= 'arm-linux-androideabi-gcc',
	cxx			= 'arm-linux-androideabi-g++',
	ar			= 'arm-linux-androideabi-ar',
	ld			= 'arm-linux-androideabi-ld',
	ranlib		= 'arm-linux-androideabi-ranlib',
	strip		= 'arm-linux-androideabi-strip',
})

insert.insert(premake.tools.gcc, 'cppflags.system.android', {
})

insert.insert(premake.tools.gcc, 'cxxflags.system.android', {
	'--sysroot=/sysroot/',
	'-fno-rtti',
	'-fno-exceptions'
})

insert.insert(premake.tools.gcc, 'ldflags.system.android', {
	'-L/sysroot/usr/lib'
})

-- print(inspect.inspect(premake.tools.gcc))

-- targetprefix "lib"
-- targetextension ".so"
-- linkoptions { "-llog", "-lGLESv1_CM", "-lz", "-s", "-shared" }

if not os.isfile(premake.tools.gcc.tools.android.cc) then
	-- error(color.fg.red ..'Cannot find GCC for Android. Make sure arm-linux-androideabi-gcc is in your PATH.' .. color.reset)
end

local this_dir = debug.getinfo(1, "S").source:match[[^@?(.*[\/])[^\/]-$]];
package.path = this_dir .. "actions/?.lua;".. package.path

require 'jni'
require 'jni_cpp'
require 'jni_solution'
require 'jni_makefile'

if os.getenv('ANDROID') then
	ANDROID = os.getenv('ANDROID');
end
