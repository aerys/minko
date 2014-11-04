-- jpeg plugin
minko.plugin.serializer = {}

function minko.plugin.serializer:enable()

    minko.plugin.enable("png")

	minko.plugin.links { "serializer" }
	includedirs {
		minko.plugin.path("serializer") .. "/include",
		minko.plugin.path("serializer") .. "/lib/msgpack-c/include" -- fixme: hide implementation
	}
	defines { "MINKO_PLUGIN_SERIALIZER" }

	configuration { "windows32" }
		links {
			"PVRTexLib",
			"QCompressLib",
			"TextureConverter"
		}

		libdirs {
			minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_32/Dynamic",
			minko.plugin.path("serializer") .. "/lib/QCompress/Lib/Win32"
		}
	
		prelinkcommands {
			minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_32/Dynamic/*.dll"),
			minko.action.copy(minko.plugin.path("serializer") .. "/lib/QCompress/Lib/Win32/*.dll")
		}

	configuration { "windows64" }
		links { "PVRTexLib" }
		libdirs { minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_64/Dynamic" }
	
		prelinkcommands {
			minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_64/Dynamic/*.dll")
		}

	configuration { "linux32" }
		links { "PVRTexLib" }
		libdirs { minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_32/Dynamic" }
	
		prelinkcommands {
			minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_32/Dynamic/*.so")
		}

	configuration { "linux64" }
		links { "PVRTexLib" }
		libdirs { minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_64/Dynamic" }
	
		prelinkcommands {
			minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_64/Dynamic/*.so")
		}

	configuration { "osx" }
		links { "PVRTexLib" }
		libdirs { minko.plugin.path("serializer") .. "/lib/PVRTexTool/OSX_x86/Dynamic" }
	
		prelinkcommands {
			minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/OSX_x86/Dynamic/*.dylib")
		}

	configuration { "linux" }
		buildoptions {
			"-Wno-deprecated-declarations"
		}
end

function minko.plugin.serializer:dist(pluginDistDir)
	-- fixme copy /lib/msgpack-c/include
	os.mkdir(pluginDistDir .. "/lib/msgpack-c/include")
	minko.os.copyfiles(minko.plugin.path("serializer") .. "/lib/msgpack-c/include", pluginDistDir .. "/lib/msgpack-c/include")
end

newoption {
	trigger		= "with-serializer",
	description	= "Enable the Minko SERIALIZER plugin."
}
