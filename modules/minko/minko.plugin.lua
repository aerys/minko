minko.plugin = {
}

minko.plugin.import = function(name)
	if minko.plugin[name] and minko.plugin[name].import then
		minko.plugin[name]:import()
	end
end

minko.plugin.include = function(path)
	if not os.isfile(path .. '/plugin.lua') then
		error(color.fg.red .. 'No plugin found in path "' .. path .. '" (imported from "' .. (project() and project().name or 'framework') .. '").' .. color.reset)
	end

	dofile(path .. '/plugin.lua')
end

minko.plugin.enable = function(name)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	configuration { unpack(terms) }

	if not minko.plugin[name] then
		error(color.fg.red .. 'Plugin "' .. name .. '" not found (enabled from "' .. (project() and project().name or 'framework') .. '").' .. color.reset)
	end

	if minko.plugin[name] and minko.plugin[name].enable then
		minko.plugin[name]:enable()
	end

	configuration { unpack(terms) }
end

-- "minko.plugin.links" is a clone of the default "links" premake function,
-- except when used from an external project. In this case, the function
-- selects the right "libdirs" based on the platform / configuration.
minko.plugin.links = function(names)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	for _, platform in ipairs(platforms()) do
		for _, cfg in ipairs(configurations()) do
			-- matching both the platform (windows32, osx64...) and the config (debug, release)
			-- but also the current scope configuration if there is one defined!
			configuration { platform, cfg, unpack(terms) }

			for _, name in ipairs(names) do
				links { "minko-plugin-" .. name }

				if MINKO_SDK_DIST then
					libdirs { minko.plugin.path(name) .. "/bin/" .. platform .. "/" .. cfg }
				end
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
