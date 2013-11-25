-- devil plugin
if minko.plugin.enabled("devil") then
	
	links { "plugin-devil" }
	defines { "MINKO_PLUGIN_DEVIL" }

	configuration { "windows" }
		links { "ColorPicker", "lcms", "libjasper", "libjpeg", "libpng", "libtiff", "zlib" }
		libdirs { minko.sdk.path("plugins/devil/lib/devil/lib/win/lib") }
		includedirs {
			minko.sdk.path("plugins/devil/src")
		}
end
