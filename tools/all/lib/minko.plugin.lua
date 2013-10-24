minko.plugin = {}

minko.plugin.import = function(name)
	local terms = configuration()["terms"]
	
	dofile(MINKO_HOME .. "/plugins/" .. name .."/plugin.lua")

	configuration { unpack(terms) }
end

minko.plugin.enable = function(name)
	_OPTIONS["with-" .. name] = "true"
	minko.plugin.import(name)
end

minko.plugin.enabled = function(name)
	return _OPTIONS["with-" .. name] ~= nil
end
