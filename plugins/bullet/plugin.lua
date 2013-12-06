-- bullet plugin
if minko.plugin.enabled("bullet") then

	minko.plugin.links { "bullet" }
	includedirs { minko.sdk.path("plugins/bullet/include") }
	defines { "MINKO_PLUGIN_BULLET" }

end