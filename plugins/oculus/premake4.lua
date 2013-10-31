newoption {
	trigger			= "with-oculus",
	description		= "Enable the Minko Oculus VR plugin."
}

minko.project.library "minko-oculus"
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c" }
	includedirs { "src" }
	
	-- libovr	
	--[[
	includedirs {
		"lib/LibOVR/Src",
		"lib/LibOVR/Include"
	}
	files {
		"lib/LibOVR/Src/OVR_DeviceHandle.cpp",
		"lib/LibOVR/Src/OVR_DeviceImpl.cpp",
		"lib/LibOVR/Src/OVR_JSON.cpp",
		"lib/LibOVR/Src/OVR_Profile.cpp",
		"lib/LibOVR/Src/OVR_SensorFilter.cpp",
		"lib/LibOVR/Src/OVR_SensorFusion.cpp",
		"lib/LibOVR/Src/OVR_SensorImpl.cpp",
		"lib/LibOVR/Src/OVR_ThreadCommandQueue.cpp",
		"lib/LibOVR/Src/OVR_DeviceHandle.h",
		"lib/LibOVR/Src/OVR_DeviceImpl.h",
		"lib/LibOVR/Src/OVR_JSON.h",
		"lib/LibOVR/Src/OVR_Profile.h",
		"lib/LibOVR/Src/OVR_SensorFilter.h",
		"lib/LibOVR/Src/OVR_SensorFusion.h",
		"lib/LibOVR/Src/OVR_SensorImpl.h",
		"lib/LibOVR/Src/OVR_ThreadCommandQueue.h"
	}
	configuration { "windows" }
		defines { "_LIB", "WIN32" }
		files {
			"lib/LibOVR/Src/OVR_Win32_DeviceManager.h",
			"lib/LibOVR/Src/OVR_Win32_DeviceStatus.h",
			"lib/LibOVR/Src/OVR_Win32_HIDDevice.h",
			"lib/LibOVR/Src/OVR_Win32_HMDDevice.h",
			"lib/LibOVR/Src/OVR_Win32_SensorDevice.h",
			"lib/LibOVR/Src/OVR_Win32_DeviceManager.cpp",
			"lib/LibOVR/Src/OVR_Win32_DeviceStatus.cpp",
			"lib/LibOVR/Src/OVR_Win32_HIDDevice.cpp",
			"lib/LibOVR/Src/OVR_Win32_HMDDevice.cpp",
			"lib/LibOVR/Src/OVR_Win32_SensorDevice.cpp"
		}
	configuration { "macosx" }
		files {
			"lib/LibOVR/Src/OVR_OSX_DeviceManager.h",
			"lib/LibOVR/Src/OVR_OSX_DeviceStatus.h",
			"lib/LibOVR/Src/OVR_OSX_HIDDevice.h",
			"lib/LibOVR/Src/OVR_OSX_HMDDevice.h",
			"lib/LibOVR/Src/OVR_OSX_SensorDevice.h",
			"lib/LibOVR/Src/OVR_OSX_DeviceManager.cpp",
			"lib/LibOVR/Src/OVR_OSX_DeviceStatus.cpp",
			"lib/LibOVR/Src/OVR_OSX_HIDDevice.cpp",
			"lib/LibOVR/Src/OVR_OSX_HMDDevice.cpp",
			"lib/LibOVR/Src/OVR_OSX_SensorDevice.cpp"
		}
	configuration { "linux" }
		files {
			"lib/LibOVR/Src/OVR_Linux_DeviceManager.h",
			"lib/LibOVR/Src/OVR_Linux_DeviceStatus.h",
			"lib/LibOVR/Src/OVR_Linux_HIDDevice.h",
			"lib/LibOVR/Src/OVR_Linux_HMDDevice.h",
			"lib/LibOVR/Src/OVR_Linux_SensorDevice.h",
			"lib/LibOVR/Src/OVR_Linux_DeviceManager.cpp",
			"lib/LibOVR/Src/OVR_Linux_DeviceStatus.cpp",
			"lib/LibOVR/Src/OVR_Linux_HIDDevice.cpp",
			"lib/LibOVR/Src/OVR_Linux_HMDDevice.cpp",
			"lib/LibOVR/Src/OVR_Linux_SensorDevice.cpp"
		}
	]]--
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
	