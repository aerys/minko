minko.sdk = {}

minko.sdk.path = function(p)

	if minko.plugin._externalPlugins[p] ~= nil then
		p = path.getabsolute(minko.plugin._externalPlugins[p] .. "/" .. p)
	else
		p = path.getabsolute(MINKO_HOME .. "/" .. p)
	end

	if os.get() == "windows" then
		p = path.translate(p, "\\")
	end

	return p
end

minko.sdk.links = function(name, path)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	for i, platform in ipairs(platforms()) do
		for j, cfg in ipairs(configurations()) do
			-- matching both the platform (windows, macosx...) and the config (debug, release)
			-- but also the current scope configuration if there is one defined!

			configuration { platform, cfg, unpack(terms) }
				libdirs { minko.sdk.path(path .. "/bin/" .. platform .. "/" .. cfg) }
				links { name }
				-- linkoptions { minko.sdk.path(path .. "/bin/" .. platform .. "/" .. cfg) .. "/lib" .. name .. ".a" }
		end
	end
end
