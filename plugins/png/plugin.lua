-- png plugin
if minko.plugin.enabled("png") then
	links { "plugin-png" }
	includedirs { minko.sdk.path("plugins/png/src") }
	defines { "MINKO_PLUGIN_PNG" }
end
