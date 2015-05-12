minko.sdk = {}

minko.sdk.path = function(p)
	-- note: cannot use path.join() here because `p` can start with '/'
	return path.getabsolute(MINKO_HOME .. "/" .. p)
end
