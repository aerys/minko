minko.plugin = {
	_enabled = {}
}

minko.plugin.import = function(name)
	if not MINKO_SDK_DIST then
		include(minko.sdk.path("/plugins/" .. name))
	end
end

minko.plugin.enable = function(name)
	local projectName = project()["name"]
	local terms = configuration()["terms"]

	minko.plugin._enabled[name] = true

	minko.plugin.import(name)
	
	project(projectName)
	configuration { unpack(terms) }
	
	dofile(minko.sdk.path("/plugins/" .. name .."/plugin.lua"))

	configuration { unpack(terms) }
end

minko.plugin.enabled = function(name)
	return minko.plugin._enabled[name] or _OPTIONS["with-" .. name] ~= nil
end

minko.plugin.links = function(names)

	local terms = configuration()["terms"]

	configuration {}

	for _, name in ipairs(names) do

		local projectName = "minko-plugin-" .. name

		if MINKO_SDK_DIST then
			configuration { 'debug' }
				links { minko.sdk.path('plugins/' .. name .. '/bin/debug/' .. minko.sdk.gettargetplatform() .. '/' .. projectName) }
			configuration { 'release' }
				links { minko.sdk.path('plugins/' .. name .. '/bin/release/' .. minko.sdk.gettargetplatform() .. '/' .. projectName) }
		else
			links { projectName }
		end

	end

	configuration { unpack(terms) }

end
