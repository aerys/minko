-- png plugin
if minko.plugin.enabled("png") then
	links { "plugin-png" }
	includedirs { minko.sdk.path("plugins/png/include") }
	defines { "MINKO_PLUGIN_PNG" }
end
