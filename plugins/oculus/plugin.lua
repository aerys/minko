-- oculus plugin
if minko.plugin.enabled("oculus") then
	local assetDir = '/plugins/oculus/asset';

	links { "minko-oculus" }
	includedirs { minko.sdk.path("plugins/oculus/src") }
	defines { "MINKO_OCULUS" }
	
	configuration { "windows" }
		postbuildcommands {
			'xcopy /y /s "' .. minko.sdk.path(assetDir) .. '\\*" "$(TargetDir)"'
		}
		
	configuration { "linux" }
	postbuildcommands {
			'cp -r ' .. minko.sdk.path(assetDir) .. '/* ${TARGETDIR}'
		}
	
	configuration { "macosx" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path(assetDir) .. '/* .'
		}

end