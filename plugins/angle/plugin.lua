-- angle plugin
if minko.plugin.enabled("angle") then
	local kind = configuration().kind
	
	configuration { "windows" }
		libdirs { minko.sdk.path("plugins/angle/lib/win/ANGLE/lib") }
		defines { "MINKO_ANGLE" }
		links { "libGLESv2", "libEGL" }
		includedirs { minko.sdk.path("plugins/angle/lib/win/ANGLE/include") }
		
		if kind ~= "StaticLib" and kind ~= "SharedLib" then
			postbuildcommands {
				minko.vs.getdllscopycommand(minko.sdk.path("plugins/angle/lib/win/ANGLE/lib"))
			}
		end
end
