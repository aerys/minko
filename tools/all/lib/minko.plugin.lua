minko.plugin = {}

minko.plugin.import = function(name)
	include(minko.sdk.path("/plugins/" .. name))
end

minko.plugin.enable = function(name)
	local projectName = project()["name"]
	local terms = configuration()["terms"]

	minko.plugin.import(name)
	
	project(projectName)
	configuration { unpack(terms) }
	
	_OPTIONS["with-" .. name] = "true"
	
	dofile(minko.sdk.path("/plugins/" .. name .."/plugin.lua"))

	configuration { unpack(terms) }
end

minko.plugin.enabled = function(name)
	return _OPTIONS["with-" .. name] ~= nil
end
