-- particles plugin
if minko.plugin.enabled("particles") then
	links { "minko-particles" }
	includedirs { minko.sdk.path("plugins/particles/src") }
	
	configuration { "windows" }
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path('/plugins/particles/asset/effect') .. '" "$(TargetDir)\\effect"',
		}
		
	configuration { "linux" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/plugins/particles/asset/effect') .. ' ${TARGETDIR} || :',
		}
	
	configuration { "macosx" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/plugins/particles/asset/effect') .. ' . || :',
		}		
	
end
