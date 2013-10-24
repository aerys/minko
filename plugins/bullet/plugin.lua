-- bullet plugin
if minko.plugin.enabled("bullet") then

	links { "minko-bullet" }
	includedirs { minko.sdk.path("plugins/bullet/src") }
	defines { "MINKO_BULLET" }

end