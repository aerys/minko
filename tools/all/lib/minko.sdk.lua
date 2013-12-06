minko.sdk = {}

minko.sdk.path = function(p)
	p = path.getabsolute(MINKO_HOME .. "/" .. p)

	if os.get() == "windows" then
		p = path.translate(p, "\\")
	end
	
	return p
end

minko.sdk.newplatform = function(platform)
  local name = platform.name
  local description = platform.description
 
  -- Register new platform
  premake.platforms[name] = {
    cfgsuffix = "_" .. name,
    iscrosscompiler = true
  }
 
  -- Allow use of new platform in --platfroms
  table.insert(premake.option.list["platform"].allowed, { name, description })
  table.insert(premake.fields.platforms.allowed, name)
 
  -- Add compiler support
  -- gcc
  premake.gcc.platforms[name] = platform.gcc
  --other compilers (?)
end

minko.sdk.gettargetplatform = function()
  if _OPTIONS['platforms'] then
    return _OPTIONS['platforms']
  else
    return os.get()
  end
end
