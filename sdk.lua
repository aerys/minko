MINKO_HOME = path.getabsolute(os.getcwd())

printf("Minko SDK home directory: " .. MINKO_HOME)

-- support for new actions
dofile(MINKO_HOME .. '/tools/all/lib/minko.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.sdk.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.os.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.path.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.plugin.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.vs.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.project.lua')

minko.sdk.newplatform {
	name = "emscripten",
	description = "Emscripten C++ to JS toolchain",
	gcc = {
		cc = path.getabsolute("../../lin/bin/emcc.sh"),
		cxx = path.getabsolute("../../lin/bin/em++.sh"),
		ar = path.getabsolute("../../lin/bin/emar.sh"),
		cppflags = "-MMD -DEMSCRIPTEN"
	}
}

minko.sdk.newplatform {
	name = "clang",
	description = "Clang",
	gcc = {
		cc = "clang",
		cxx = "clang++",
		ar = "ar",
		cppflags = "-MMD",
		ldflags = "-stdlib=libc++"
	}
}

if _OPTIONS.platform then
	print("Selected target platform: " .. _OPTIONS["platform"])
    -- overwrite the native platform with the options::platform
    premake.gcc.platforms['Native'] = premake.gcc.platforms[_OPTIONS.platform]
end
