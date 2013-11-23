-- lua plugin
if minko.plugin.enabled("lua") then
	links { "plugin-lua" }
	includedirs {
		minko.sdk.path("plugins/lua/src"),
		minko.sdk.path("plugins/lua/binding")
	}
	defines { "MINKO_PLUGIN_LUA" }
end
