-- jpeg plugin
if minko.plugin.enabled("fx") then
	defines { "MINKO_PLUGIN_FX" }
	
	configuration { "windows" }
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path('/plugins/fx/asset/effect') .. '" "$(TargetDir)\\effect"',
		}
		
	configuration { "linux" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/plugins/fx/asset/effect') .. ' ${TARGETDIR} || :',
		}
	
	configuration { "macosx" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/plugins/fx/asset/effect') .. ' . || :',
		}		

	
end
