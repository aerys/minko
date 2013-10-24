-- jpeg plugin
if minko.plugin.enabled("jpeg") then

	links { "minko-jpeg" }
	includedirs { minko.sdk.path("plugins/jpeg/src") }
	defines { "MINKO_JPEG" }

end