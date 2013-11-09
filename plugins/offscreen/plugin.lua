-- offscreen plugin
if minko.plugin.enabled("offscreen") then

	defines { "MINKO_OFFSCREEN" }

	libdirs { "/opt/local/lib" }
	
	links { "minko-offscreen", "OSMesa" }
	includedirs {
		"/opt/local/include",
		minko.sdk.path("plugins/sdl/lib/osmesa/include"),
		minko.sdk.path("plugins/offscreen/src")
	}
end
