minko.plugin = {
}

minko.plugin.import = function(name)
	if minko.plugin[name] and minko.plugin[name].import then
		minko.plugin[name]:import()
	end
end

minko.plugin.include = function(path)
	if not MINKO_SDK_DIST then
		include(path)
	end

	dofile(path .. '/plugin.lua')
end

minko.plugin.enable = function(name)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	configuration { unpack(terms) }

	if not minko.plugin[name] then
		error(color.fg.red .. 'Plugin "' .. name .. '" not found (enabled from "' .. project().name .. '").' .. color.reset)
	end

	if minko.plugin[name] and minko.plugin[name].enable then
		minko.plugin[name]:enable()
	end

	configuration { unpack(terms) }
end

minko.plugin.links = function(names)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	for _, name in ipairs(names) do
		configuration { unpack(terms) }

		local plugin = "minko-plugin-" .. name

		if not MINKO_SDK_DIST then
			links { plugin }
		else
			if minko.plugin[name] and minko.plugin[name].links then
				minko.plugin[name]:links()
			end
		end
	end

	configuration { unpack(terms) }
end

minko.plugin.path = function(name)
	return minko.plugin[name]._path
end

setmetatable(minko.plugin, {
	__newindex = function(t, k, v)
		assert(type(v) == "table", 'The correct syntax for initializing a plugin is: minko.plugin.name = {}')
		v._path = os.getcwd()
		rawset(t, k, v)
	end
})
