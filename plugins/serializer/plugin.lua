-- jpeg plugin
minko.plugin.serializer = {}

function minko.plugin.serializer:enable()
	minko.plugin.links { "serializer" }
	includedirs { 
		minko.sdk.path("plugins/serializer/include"),
		minko.sdk.path("plugins/serializer/lib/msgpack-c/include") }
	defines { "MINKO_PLUGIN_SERIALIZER" }

	configuration { "linux" }
		buildoptions {
			"-Wno-deprecated-declarations"
		}
end

function minko.plugin.serializer:dist(pluginDistDir)
	-- fixme copy /lib/msgpack-c/include
	os.mkdir(pluginDistDir .. "/lib/msgpack-c/include")
	minko.os.copyfiles(minko.sdk.path("plugins/serializer/lib/msgpack-c/include"), pluginDistDir .. "/lib/msgpack-c/include")
end