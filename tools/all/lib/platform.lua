newplatform {
	name = "emscripten",
	description = "Emscripten C++ to JS toolchain",
	gcc = {
		cc = path.getabsolute("../../lin/bin/emcc.sh"),
		cxx = path.getabsolute("../../lin/bin/em++.sh"),
		ar = path.getabsolute("../../lin/bin/emar.sh"),
		cppflags = "-MMD -DEMSCRIPTEN"
	}
}

newplatform {
	name = "clang",
	description = "Clang",
	gcc = {
		cc = "clang",
		cxx = "clang++",
		ar = "ar",
		cppflags = "-MMD "
	}
}

if _OPTIONS.platform then
	print("Selected target platform: " .. _OPTIONS["platform"])
    -- overwrite the native platform with the options::platform
    premake.gcc.platforms['Native'] = premake.gcc.platforms[_OPTIONS.platform]
end
