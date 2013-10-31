-- bullet plugin
if minko.plugin.enabled("bullet") then

	links { "plugin-bullet" }
	includedirs { minko.sdk.path("plugins/bullet/src") }
	defines { "MINKO_PLUGIN_BULLET" }

end