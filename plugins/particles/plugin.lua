-- particles plugin
if minko.plugin.enabled("particles") then
	links { "plugin-particles" }
	includedirs { minko.sdk.path("plugins/particles/include") }
	defines { "MINKO_PLUGIN_PARTICES" }
	
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
