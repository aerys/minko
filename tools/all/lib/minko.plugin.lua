minko.plugin = {
	_enabled = {}
}

minko.plugin.import = function(name)
	if not MINKO_SDK_DIST then
		include(minko.sdk.path("/plugins/" .. name))
	end

	if minko.plugin[name] and minko.plugin[name].import then
		minko.plugin[name]:import()
	end
end

minko.plugin.enable = function(name)
	local cfg = configuration().configset._current

	local projectName = project().name
	local terms = cfg._criteria.terms

	minko.plugin._enabled[name] = true
	minko.plugin.import(name)	
	
	project(projectName)
	configuration { unpack(terms) }

	dofile(minko.sdk.path("/plugins/" .. name .."/plugin.lua"))

	if minko.plugin[name] and minko.plugin[name].enable then
		minko.plugin[name]:enable()
	end

	configuration { unpack(terms) }
end

minko.plugin.enabled = function(name)
	return minko.plugin._enabled[name] or _OPTIONS["with-" .. name] ~= nil
end

minko.plugin.links = function(names)
	local cfg = configuration().configset._current

	local projectName = project().name
	local terms = cfg._criteria.terms

	for _, name in ipairs(names) do
		configuration { unpack(terms) }

		local projectName = "minko-plugin-" .. name

		if MINKO_SDK_DIST then
			minko.sdk.links(projectName, "plugins/" .. name)
		else
			links { projectName }
		end
	end

	configuration { unpack(terms) }
end
