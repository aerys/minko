-- jpeg plugin
minko.plugin.serializer = {}

function minko.plugin.serializer:enable()
	minko.plugin.links { "serializer" }
	includedirs { 
		minko.plugin.path("serializer") .. "/include",
		minko.plugin.path("serializer") .. "/lib/msgpack-c/include" -- fixme: hide implementation
	}
	defines { "MINKO_PLUGIN_SERIALIZER" }

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
