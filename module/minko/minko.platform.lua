minko.platform = {}

minko.platform.platforms = function(t)
	for _, v in pairs(t) do
		if minko.platform.supports(v) then
			platforms(v)
		end
	end
end

minko.platform.supports = function(target)
	if target == "html5" then
		if EMSCRIPTEN and string.startswith(_ACTION, "gmake") then
			return true
		end
	elseif target == "linux32" or target == "linux64" then
		if os.is("linux") then
			return true
		end
	elseif target == "windows32" or target == "windows64" then
		if os.is("windows") then
			return true
		end
	elseif target == "osx64" then
		if os.is("macosx") and string.startswith(_ACTION, "gmake") then
			return true
		end
	elseif target == "ios" then
		if os.is("macosx") and string.startswith(_ACTION, "xcode") then
			return true
		end
	elseif target == "android" then
		if NDK_HOME and string.startswith(_ACTION, "gmake") then
			return true
		end
	end
	return false
end
