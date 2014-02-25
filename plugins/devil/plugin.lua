-- devil plugin

minko.plugin.devil = {}

function minko.plugin.devil:enable()
	minko.plugin.links { "devil" }
	includedirs { minko.sdk.path("plugins/devil/src") }
	defines { "MINKO_PLUGIN_DEVIL" }
end
