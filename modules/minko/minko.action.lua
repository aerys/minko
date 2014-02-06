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
	if not os.isfile("sdk.lua") then
		error("cannot clean from outside the Minko SDK")
	end

	os.execute("git clean -X -n")
	
	for _, pattern in ipairs { "framework", "plugins/*", "tests", "examples/*" } do
		local dirs = os.matchdirs(pattern)

		for _, dir in ipairs(dirs) do
			local cwd = os.getcwd()
			os.chdir(dir)
			os.execute("git clean -X -n")
			os.chdir(cwd)
		end
	end
end
