-- mk plugin
if minko.plugin.enabled("mk") then

	links { "plugin-mk" }
	includedirs {
		minko.sdk.path("plugins/mk/src"),
		minko.sdk.path("plugins/mk/lib/msgpack-c/src")
	}
	defines { "MINKO_PLUGIN_MK" }
	
end