minko.sdk = {}

minko.sdk.path = function(p)
	p = path.getabsolute(MINKO_HOME .. "/" .. p)

	if os.get() == "windows" then
		p = path.translate(p, "\\")
	end
	
	return p
end

minko.sdk.gettargetplatform = function()
  if _OPTIONS['platforms'] then
    return _OPTIONS['platforms']
  else
    return os.get()
  end
end

minko.sdk.links = function(project)
	for i, platform in ipairs(platforms()) do
		for j, cfg in ipairs(configurations()) do
			configuration { platform, cfg }
				links { minko.sdk.path("framework/bin/" .. platform .. "/" .. cfg .. "/" .. project) }
		end
	end
end
