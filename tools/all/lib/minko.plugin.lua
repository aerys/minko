minko.plugin = {
	_enabled = {},
	_externalPlugins = {}
}

-- register external plugin
minko.plugin.register = function(name)
	print ("register " .. name)
	minko.plugin._externalPlugins[name] = os.getcwd()
end

minko.plugin.import = function(name)
	if not MINKO_SDK_DIST then

		if os.isdir(minko.sdk.path("/plugins/" .. name)) then
			include(minko.sdk.path("/plugins/" .. name))
		else
			include(minko.plugin._externalPlugins[name])
		end
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

	print ("Minko enable " .. name .. " "  .. os.getcwd())

	if os.isdir(minko.sdk.path("/plugins/" .. name)) then
		dofile(minko.sdk.path("/plugins/" .. name .."/plugin.lua"))
	elseif minko.plugin._externalPlugins[name] ~= nil then
		dofile(minko.plugin._externalPlugins[name] .. "/plugin.lua")
	else
		print(color.fg.red ..'Plugin ' .. name .. " not found." .. color.reset)
		os.exit(1)
	end
	if minko.plugin[name] and minko.plugin[name].enable then
		minko.plugin[name]:enable()
	end

	configuration { unpack(terms) }
end

minko.plugin.enabled = function(name)
	return minko.plugin._enabled[name] or _OPTIONS["with-" .. name] ~= nil
end

minko.plugin.path = function(name)
			
	if minko.plugin._externalPlugins[name] ~= nil then
		name = path.getabsolute(minko.plugin._externalPlugins[name])
	else
		name = path.getabsolute(MINKO_HOME .. "/plugins/" .. name)
	end

	if os.get() == "windows" then
		name = path.translate(name, "\\")
	end
	
	return name
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
