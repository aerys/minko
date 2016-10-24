PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

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

	configuration { "android or ios or html5" }
		minko.plugin.enable("sensors")

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
		buildoptions { "-x objective-c++" }
		files {
			"lib/libovr/osx/**.cpp",
			"lib/libovr/osx/src/Util/Util_SystemInfo_OSX.mm"
		}
		includedirs {
			"lib/libovr/osx/src",
			"lib/libovr/osx/include",
		}

	configuration { "not html5" }
		excludes {
			"include/minko/vr/WebVR.hpp",
			"src/minko/vr/WebVR.cpp",
		}

	configuration { "not android", "not ios", "not html5" }
		excludes {
			"include/minko/vr/Cardboard.hpp",
			"src/minko/vr/Cardboard.cpp",
		}

	configuration { "html5 or android or ios" }
		excludes {
			"include/minko/vr/OculusRift.hpp",
			"src/minko/vr/OculusRift.cpp",
		}
