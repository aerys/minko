minko.plugin = {}

minko.plugin.include = function(directory, enabled)
	local name = path.getname(directory)

	if not os.isfile(directory .. '/plugin.lua') then
		print(color.fg.red .. 'Warning: no plugin found in path "' .. directory .. '" (imported from "' .. (project() and project().name or 'framework') .. '").' .. color.reset)
		do return end
	end

	newoption {
		trigger = 'with-' .. name,
		description = 'Enable plugin "' .. name .. '"'
	}

	newoption {
		trigger	= 'without-' .. name,
		description = 'Disable plugin "' .. name .. '"'
	}

	dofile(directory .. '/plugin.lua')

	minko.plugin[name]._enabled = enabled
end

minko.plugin.enable = function(names)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	if type(names) == "string" then
		names = { names }
	end

	for _, name in ipairs(names) do
		configuration { unpack(terms) }

		if not minko.plugin[name] then
			error(color.fg.red .. 'Plugin "' .. name .. '" not found (enabled from "' .. (project() and project().name or 'framework') .. '").' .. color.reset)
		end

		if minko.plugin[name] and minko.plugin[name].enable then
			minko.plugin[name]:enable()
		end
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


				if platform == "android" then
					prelinkcommands {
						minko.action.cpjf(minko.plugin.path(name) .. '/src/', '${TARGETDIR}/src/com/minko/')
					}
				end

				links { "minko-plugin-" .. name }

				if MINKO_SDK_DIST then
					libdirs { minko.plugin.path(name) .. "/bin/" .. platform .. "/" .. cfg }
				end
			end
		end
	end

	configuration { unpack(terms) }
end

minko.plugin.list = function()
	local plugins = {}

	for name, data in pairs(minko.plugin) do
		if (type(data) == 'table') then
			table.insert(plugins, name)
		end
	end

	return plugins
end

minko.plugin.path = function(name)
	return minko.plugin[name]._path
end

minko.plugin.available = function(name)
	return minko.plugin[name] ~= nil
end

minko.plugin.requested = function(name)
	return _OPTIONS['with-' .. name] or (minko.plugin[name]._enabled and not _OPTIONS['without-' .. name])
end

setmetatable(minko.plugin, {
	__newindex = function(t, k, v)
		assert(type(v) == "table", 'The correct syntax for initializing a plugin is: minko.plugin.name = {}')
		v._name = k
		v._path = os.getcwd()
		v._enabled = false
		rawset(t, k, v)
	end
})
