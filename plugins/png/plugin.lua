-- png plugin
if minko.plugin.enabled("png") then
	configuration { "debug" }
		links { "minko-png" }
		includedirs { minko.sdk.path("plugins/png/src") }
		
	configuration { "release" }
		links { "minko-png" }
		includedirs { minko.sdk.path("plugins/png/src") }
end
