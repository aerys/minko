minko.sdk = {}

minko.sdk.path = function(p)
	return path.translate(path.getabsolute(MINKO_HOME .. "/" .. p))
end
