minko.sdk = {}

minko.sdk.path = function(p)
	p = path.getabsolute(MINKO_HOME .. "/" .. p)

	if os.get() == "windows" then
		p = path.translate(p, "\\")
	end
	
	return p
end

minko.sdk.links = function(project, path)
	local cfg = configuration().configset._current
	local terms = cfg._criteria.terms

	for i, platform in ipairs(platforms()) do
		for j, cfg in ipairs(configurations()) do
			-- matching both the platform (windows, macosx...) and the config (debug, release)
			-- but also the current scope configuration if there is one defined!
			configuration { platform, cfg, unpack(terms) }
				libdirs { minko.sdk.path(path .. "/bin/" .. platform .. "/" .. cfg) }
				links { project }
				-- linkoptions { minko.sdk.path(path .. "/bin/" .. platform .. "/" .. cfg) .. "/lib" .. project .. ".a" }
		end
	end
end
