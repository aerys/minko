PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64" } then
	return
end

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "ios", "android" }

	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp",
        "asset/**"
	}
	includedirs {
		"include",
		"src",
		"lib/opengl/include",
	}

	configuration { "windows32 or windows64" }
		files {
			"lib/libovr/windows/**.cpp"
		}
		includedirs {
			"lib/libovr/windows/src",
			"lib/libovr/windows/include",
		}
		excludes {
			"lib/libovr/windows/src/CAPI/D3D1X/CAPI_D3D1X_Util.*",
			"lib/libovr/windows/src/CAPI/D3D1X/CAPI_D3D1X_DistortionRenderer.*"
		}
		defines {
			"UNICODE",
			"_UNICODE",
			"WIN32"
		}

	configuration { "linux32 or linux64" }
		files {
			"lib/libovr/linux/**.cpp"
		}
		includedirs {
			"lib/libovr/linux/src",
			"lib/libovr/linux/include"
		}

	configuration { "osx64" }
		files {
			"lib/libovr/osx/**.cpp"
		}
		includedirs {
			"lib/libovr/osx/src",
			"lib/libovr/osx/include",
		}

	configuration { "not html5" }
		excludes {
			"include/minko/oculus/WebVROculus.hpp",
			"src/minko/oculus/WebVROculus.cpp",
		}

	configuration { "html5" }
		excludes {
			"include/minko/oculus/NativeOculus.hpp",
			"src/minko/oculus/NativeOculus.cpp",
		}
