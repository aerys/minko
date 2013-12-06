-- assimp plugin
if minko.plugin.enabled("assimp") then
	minko.plugin.links { "assimp" }
	includedirs { minko.sdk.path("plugins/assimp/include") }
	defines { "MINKO_PLUGIN_ASSIMP" }
end
