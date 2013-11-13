-- assimp plugin
if minko.plugin.enabled("assimp") then
	links { "plugin-assimp" }
	includedirs { minko.sdk.path("plugins/assimp/src") }
	defines { "MINKO_PLUGIN_ASSIMP" }
end
