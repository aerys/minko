minko.sdk = {}

minko.sdk.path = function(p)
	p = path.getabsolute(MINKO_HOME .. "/" .. p)

	if os.is('windows') then
		return path.translate(p)
	else
		return p
	end
end
