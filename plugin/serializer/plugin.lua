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

	if _OPTIONS['with-texture-compressor'] then
		configuration { "windows32" }
			links {
				"PVRTexLib",
				"QCompressLib",
				"TextureConverter"
			}

			libdirs {
				minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_32/Dynamic",
				minko.plugin.path("serializer") .. "/lib/QCompress/Lib/windows/Win32"
			}

			prelinkcommands {
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_32/Dynamic/*.dll"),
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/QCompress/Lib/windows/Win32/*.dll")
			}

		configuration { "windows64" }
			links {
				"PVRTexLib",
				"TextureConverter"
			}

			libdirs {
				minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_64/Dynamic",
				minko.plugin.path("serializer") .. "/lib/QCompress/Lib/windows/x64"
			}

			prelinkcommands {
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Windows_x86_64/Dynamic/*.dll"),
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/QCompress/Lib/windows/x64/*.dll")
			}

		configuration { "linux32" }
			links {
				"PVRTexLib",
				"TextureConverter",
				"QCompressLib"
			}

			libdirs {
				minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_32/Dynamic",
				minko.plugin.path("serializer") .. "/lib/QCompress/Lib/ubuntu/i386"
			}

			prelinkcommands {
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_32/Dynamic/*.so"),
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/QCompress/Lib/ubuntu/i386/*.so")
			}

		configuration { "linux64" }
			links {
				"PVRTexLib"
			}

			libdirs {
				minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_64/Dynamic"
			}

			prelinkcommands {
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/Linux_x86_64/Dynamic/*.so")
			}

		configuration { "linux" }
			linkoptions {
				"-Wl,-rpath=."
			}			

		configuration { "osx32" }
			links {
				"PVRTexLib",
				"QCompressLib"
			}

			libdirs {
				minko.plugin.path("serializer") .. "/lib/PVRTexTool/OSX_x86/Dynamic",
				minko.plugin.path("serializer") .. "/lib/QCompress/Lib/osx/x86"
			}

			prelinkcommands {
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/OSX_x86/Dynamic/*.dylib"),
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/QCompress/Lib/osx/x86/*.dylib")
			}

		configuration { "osx64" }
			links {
				"PVRTexLib",
				"QCompressLib"
			}

			libdirs {
				minko.plugin.path("serializer") .. "/lib/PVRTexTool/OSX_x86/Dynamic",
				minko.plugin.path("serializer") .. "/lib/QCompress/Lib/osx/x64"
			}

			prelinkcommands {
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/PVRTexTool/OSX_x86/Dynamic/*.dylib"),
				minko.action.copy(minko.plugin.path("serializer") .. "/lib/QCompress/Lib/osx/x64/*.dylib")
			}
	end

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

newoption {
	trigger	= "with-texture-compressor",
	description = "Enable the Minko SERIALIZER plugin to support texture compression."
}
