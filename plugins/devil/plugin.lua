-- devil plugin
if minko.plugin.enabled("devil") then
	links { "plugin-devil" }
	defines { "MINKO_PLUGIN_DEVIL" }
	includedirs { minko.sdk.path("plugins/devil/src") }
end
