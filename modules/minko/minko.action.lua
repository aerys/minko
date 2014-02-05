minko.action = {}

minko.action.fail = function()
	return 'source ' .. minko.sdk.path('/tools/lin/bin/fail.sh') .. ' ${TARGET}'
end

minko.action.copy = function(sourcePath)
	if os.is('windows') then
		return 'xcopy /y /i /e "' .. path.translate(sourcePath) .. '" "$(TargetDir)"'
	else
		return 'cp -R "' .. sourcePath .. '" ${TARGETDIR}'
	end
end

minko.action.clean = function()
	error("Not implemented yet")
end
