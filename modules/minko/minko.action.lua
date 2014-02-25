minko.action = {}

minko.action.fail = function()
	if os.is('windows') then
		if string.startswith(_ACTION, "gmake") then
			return 'call "' .. path.translate(minko.sdk.path('/tools/win/scripts/fail.bat')) .. '" ${TARGET}'
		else
			return 'call "' .. path.translate(minko.sdk.path('/tools/win/scripts/fail.bat')) .. '" "$(Target)"'
		end
	elseif os.is('macosx') then
		return 'bash ' .. minko.sdk.path('/tools/mac/scripts/fail.sh') .. ' ${TARGET}'		
	else
		return 'bash ' .. minko.sdk.path('/tools/lin/scripts/fail.sh') .. ' ${TARGET}'
	end
end

minko.action.copy = function(sourcePath)
	if os.is('windows') then
		sourcePath = path.translate(sourcePath)

		if string.startswith(_ACTION, "gmake") then
			return 'xcopy /y /i /e "' .. sourcePath .. '" $(subst /,\\,$(TARGETDIR))'
		else
			return 'xcopy /y /i /e "' .. sourcePath .. '" "$(TargetDir)"'
		end
		-- return 'if exist ' .. sourcePath .. ' xcopy /y /i /e "' .. sourcePath .. '" "$(TargetDir)"'
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

minko.action.zip = function(directory, archive)
	if os.is('windows') then
		os.execute('7za a "' .. archive .. '" "' .. path.translate(directory) .. '"')
	else
		os.execute('zip -r "' .. archive .. '" "' .. directory .. '"')
	end
end
