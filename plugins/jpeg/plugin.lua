-- jpeg plugin
if minko.plugin.enabled("jpeg") then

	minko.plugin.links { "jpeg" }
	includedirs { minko.sdk.path("plugins/jpeg/include") }
	defines { "MINKO_PLUGIN_JPEG" }

end
