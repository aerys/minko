-- webgl plugin
if minko.plugin.enabled("webgl") then
	defines { "MINKO_PLUGIN_WEBGL" }
	configuration { "emscripten" }
		links { "plugin-webgl" }
		includedirs { minko.sdk.path("plugins/webgl/include") }
end
