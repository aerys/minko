-- leap plugin
if minko.plugin.enabled("leap") then
	local kind = configuration().kind
	
	configuration { "windows" }
		libdirs { minko.sdk.path("plugins/leap/lib/win/leap/lib") }
		links { "Leap", "Leapd" }
		includedirs { minko.sdk.path("plugins/leap/lib/include") }
		
		if kind ~= "StaticLib" and kind ~= "SharedLib" then
			postbuildcommands {
				minko.vs.getdllscopycommand(minko.sdk.path("plugins/leap/lib/win/leap/lib"))
			}
		end
end
