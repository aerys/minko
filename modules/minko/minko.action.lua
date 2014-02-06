minko.action = {}

minko.action.fail = function()
	if os.is('windows') then
		return 'call "' .. path.translate(minko.sdk.path('/tools/win/scripts/fail.bat')) .. '" "$(Target)"'
	elseif os.is('macosx') then
		return 'source ' .. minko.sdk.path('/tools/mac/scripts/fail.sh') .. ' ${TARGET}'		
	else
		return 'source ' .. minko.sdk.path('/tools/lin/scripts/fail.sh') .. ' ${TARGET}'
	end
end

minko.action.copy = function(sourcePath)
	if os.is('windows') then
		sourcePath = path.translate(sourcePath)
		return 'if exist ' .. sourcePath .. ' xcopy /y /i /e "' .. sourcePath .. '" "$(TargetDir)"'
	else
		return 'test -e "' .. sourcePath .. '" && cp -R "' .. sourcePath .. '" "${TARGETDIR}" || :'
	end
end

minko.action.clean = function()
	if not os.isfile("sdk.lua") then
		error("cannot clean from outside the Minko SDK")
	end

	local cmd = "git clean -X -f"

	os.execute(cmd)
	
	for _, pattern in ipairs { "framework", "plugins/*", "tests", "examples/*" } do
		local dirs = os.matchdirs(pattern)

		for _, dir in ipairs(dirs) do
			local cwd = os.getcwd()
			os.chdir(dir)
			os.execute(cmd)
			os.chdir(cwd)
		end
	end
end
