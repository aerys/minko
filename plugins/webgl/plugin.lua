-- webgl plugin
if minko.plugin.enabled("webgl") then
	defines { "MINKO_PLUGIN_WEBGL" }
	configuration { "emscripten" }
		minko.plugin.links { "webgl" }
		includedirs { minko.sdk.path("plugins/webgl/include") }
end
