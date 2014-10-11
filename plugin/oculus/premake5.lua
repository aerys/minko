PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64" } then
	return
end

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("serializer")
	
	removeplatforms { "ios", "android" }

	files {
		"**.hpp",
		"**.h",
		"**.cpp",
		"**.c",
		"include/**.hpp",
		"lib/LibOVR/Src/**",
        "asset/**"
	}
	includedirs {
		"include",
		"src",
		"lib/opengl/include",
		"lib/LibOVR/Include",
		"lib/LibOVR/Src"
	}

	excludes {
		"lib/LibOVR/Include/OVRVersion.h"
	}

	configuration { "windows32 or windows64" }
		excludes {
			"lib/LibOVR/Src/CAPI/D3D1X/CAPI_D3D1X_Util.*",
			"lib/LibOVR/Src/CAPI/D3D1X/CAPI_D3D1X_DistortionRenderer.*"
		}
		defines {
			"UNICODE",
			"_UNICODE",
			"WIN32"
		}

	configuration { "linux32 or linux64" }
		includedirs {
			"lib/lin/Xinerama/include",
			"lib/lin/libudev/include"
		}
		excludes {
			"lib/LibOVR/Src/OVR_Win32_*",
			"lib/LibOVR/Src/OVR_OSX_*",
			"lib/LibOVR/Src/Kernel/OVR_ThreadsWinAPI.cpp"
		}

	configuration { "osx64" }
		excludes {
			"lib/LibOVR/Src/OVR_Win32_*",
			"lib/LibOVR/Src/OVR_Linux_*",
			"lib/LibOVR/Src/Kernel/OVR_ThreadsWinAPI.cpp"
		}
		
	configuration { "html5" }
		excludes {
			"include/minko/oculus/NativeOculus.hpp",
			"src/minko/oculus/NativeOculus.cpp",
			"lib/LibOVR/**"
		}
