minko.worker = {
}

minko.worker.include = function(path)
	if not os.isfile(path .. '/worker.lua') then
		error(color.fg.red .. 'No worker found in path "' .. path .. '" (imported from "' .. (project() and project().name or 'framework') .. '").' .. color.reset)
	end

	dofile(path .. '/worker.lua')
end

minko.worker.enable = function(names)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	if type(names) == "string" then
		names = { names }
	end

	for _, name in ipairs(names) do
		configuration { unpack(terms) }

		if not minko.worker[name] then
			error(color.fg.red .. 'Worker "' .. name .. '" not found (enabled from "' .. (project() and project().name or 'framework') .. '").' .. color.reset)
		end

		if minko.worker[name] and minko.worker[name].enable then
			minko.worker[name]:enable()
		end
	end

	configuration { unpack(terms) }
end

-- "minko.worker.links" is a clone of the default "links" premake function,
-- except when used from an external project. In this case, the function
-- selects the right "libdirs" based on the platform / configuration.
minko.worker.links = function(names)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	for _, platform in ipairs(platforms()) do
		for _, cfg in ipairs(configurations()) do
			configuration { platform, cfg, unpack(terms) }
				for _, name in ipairs(names) do
					links { "minko-worker-" .. name }

					if MINKO_SDK_DIST then
						libdirs { minko.worker.path(name) .. "/bin/" .. platform .. "/" .. cfg }
					end
				end

			configuration { "html5", cfg, unpack(terms) }
				for _, name in ipairs(names) do
					postbuildcommands {
						minko.action.copy(minko.worker.path(name) .. "/bin/" .. platform .. "/" .. cfg .. "/" .. "minko-worker-" .. name .. ".js")
					}
				end
		end
	end

	configuration { unpack(terms) }
end

minko.worker.path = function(name)
	return minko.worker[name]._path
end

setmetatable(minko.worker, {
	__newindex = function(t, k, v)
		assert(type(v) == "table", 'The correct syntax for initializing a worker is: minko.worker.name = {}')
		v._path = os.getcwd()
		rawset(t, k, v)
	end
})
