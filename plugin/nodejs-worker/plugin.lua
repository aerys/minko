-- nodejs worker plugin
minko.plugin["nodejs-worker"] = {}

minko.plugin["nodejs-worker"].enable = function (self)
	includedirs { minko.plugin.path("nodejs-worker") .. "/include" }

	defines { "MINKO_PLUGIN_NODEJS_WORKER" }

	configuration { "not html5" }
		minko.plugin.links { "nodejs-worker" }
end
