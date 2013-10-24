-- webgl plugin
if minko.plugin.enabled("webgl") then
	configuration { "emscripten" }
		links { "minko-webgl" }
		includedirs { minko.sdk.path("plugins/webgl/src") }
end
