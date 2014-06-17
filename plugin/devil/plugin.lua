-- devil plugin

minko.plugin.devil = {}

function minko.plugin.devil:enable()
	minko.plugin.links { "devil" }
	includedirs { minko.plugin.path("devil") .. "/include" }
	defines { "MINKO_PLUGIN_DEVIL" }
	
	minko.plugin.enable("zlib")
end

newoption {
	trigger			= "with-devil",
	description		= "Enable the Minko DevIL plugin."
}
