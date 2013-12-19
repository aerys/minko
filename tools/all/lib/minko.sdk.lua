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
