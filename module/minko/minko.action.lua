minko.action = {}

minko.action.fail = function()
	if os.is('windows') then
		if string.startswith(_ACTION, "gmake") then
			return 'call "' .. path.translate(minko.sdk.path('/tool/win/script/fail.bat')) .. '" ${TARGET}'
		else
			return 'call "' .. path.translate(minko.sdk.path('/tool/win/script/fail.bat')) .. '" "$(Target)"'
		end
	elseif os.is('macosx') then
		return 'bash ' .. minko.sdk.path('/tool/mac/script/fail.sh') .. ' ${TARGET}'		
	else
		return 'bash ' .. minko.sdk.path('/tool/lin/script/fail.sh') .. ' ${TARGET}'
	end
end

minko.action.copy = function(sourcePath)
	if os.is('windows') then
		sourcePath = path.translate(sourcePath)

		local targetDir = string.startswith(_ACTION, "gmake") and '$(subst /,\\,$(TARGETDIR))' or '"$(TargetDir)"'

		if os.isdir(sourcePath) then
			targetDir = targetDir .. '\\' .. path.getbasename(sourcePath)
		end

		local existenceTest = string.find(sourcePath, '*') and '' or ('if exist ' .. sourcePath .. ' ')

		return existenceTest .. 'xcopy /y /i /e "' .. sourcePath .. '" ' .. targetDir
	else
		local targetDir = string.startswith(_ACTION, "xcode") and '${TARGET_BUILD_DIR}/${TARGET_NAME}.app' or '${TARGETDIR}'

		return 'test -e ' .. sourcePath .. ' && cp -R ' .. sourcePath .. ' "' .. targetDir .. '" || :'
	end
end

minko.action.link = function(sourcePath)
	if os.is('windows') then
		-- fixme: not needed yet
	else
		local targetDir = string.startswith(_ACTION, "xcode") and '${TARGET_BUILD_DIR}/${TARGET_NAME}.app' or '${TARGETDIR}'

		return 'test -e ' .. sourcePath .. ' && ln -s -f ' .. sourcePath .. ' "' .. targetDir .. '" || :'
	end
end

minko.action.clean = function()
	if not os.isfile("sdk.lua") then
		error("cannot clean from outside the Minko SDK")
	end

	local cmd = 'git clean -X -d -f'

	os.execute(cmd)
	
	for _, pattern in ipairs { "framework", "plugin/*", "test", "example/*" } do
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
