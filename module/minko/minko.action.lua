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

minko.action.copy = function(sourcepath, destpath)
	-- print('minko.action.copy(' .. sourcepath .. ')')

	local iscygwin = not string.startswith(os.getenv('OSTYPE'), 'CYGWIN')

	if os.is('windows') and not iscygwin then

		sourcepath = path.translate(sourcepath)

		local targetdir = string.startswith(_ACTION, "gmake") and '$(subst /,\\,$(TARGETDIR))' or '$(TargetDir)'

		destpath = destpath and path.join(targetdir, destpath) or targetdir

		if os.isdir(sourcepath) then
			destpath = path.join(destpath, path.getbasename(sourcepath))
		end

		-- print(' -> xcopy /y /i /e "' .. path.translate(sourcepath) .. '" "' .. path.translate(destpath) .. '"')

		return 'mkdir "' .. path.translate(path.getdirectory(destpath)) .. '" & ' ..
			   'xcopy /y /e "' .. path.translate(sourcepath) .. '" "' .. path.translate(destpath) .. '"'

	elseif os.is("macosx") then

		local targetdir = '${TARGETDIR}'

		if (_ACTION == "xcode-osx") then
			targetdir = '${TARGET_BUILD_DIR}'
		elseif (_ACTION == "xcode-ios") then
			targetdir = '${TARGET_BUILD_DIR}/${TARGET_NAME}.app'
		end

		if not destpath then
			destpath = path.getname(sourcepath)
		end

		destpath = path.join(targetdir, destpath)

		-- local destdir = os.isdir(sourcepath) and destpath or path.getdirectory(destpath)

		if os.isdir(sourcepath) and not string.endswith(sourcepath, '/') then
			sourcepath = sourcepath .. '/' -- cp will copy the content of the directory
		end

		-- print(' -> cp -R ' .. sourcepath .. ' "' .. destpath .. '"')

		return 'mkdir -p "' .. path.getdirectory(destpath) .. '"; ' ..
			   'cp -R "' .. sourcepath .. '" "' .. destpath .. '"'

	else

		local targetdir = '${TARGETDIR}'

		if iscygwin then
			sourcepath = os.capture('cygpath -u "' .. sourcepath .. '"')
			targetdir = os.capture('cygpath -u "' .. targetdir .. '"')
		end

		destpath = destpath and path.join(targetdir, destpath) or targetdir

		return 'mkdir -p "' .. path.getdirectory(destpath) .. '"; ' ..
			   'cp -R "' .. sourcepath .. '" "' .. destpath .. '"'

	end
end

minko.action.link = function(sourcepath)
	local iscygwin = not string.startswith(os.getenv('OSTYPE'), 'CYGWIN')

	if os.is('windows') and not iscygwin then
		-- fixme: not needed yet
	elseif os.is("macosx") then
		local targetdir = '${TARGETDIR}'

		if (_ACTION == "xcode-osx") then
			targetdir = '${TARGET_BUILD_DIR}'
		elseif (_ACTION == "xcode-ios") then
			targetdir = '${TARGET_BUILD_DIR}/${TARGET_NAME}.app'
		end

		local exists = string.find(sourcepath, '*') and '' or ('test -e ' .. sourcepath .. ' && ')

		destpath = destpath and path.join(targetdir, destpath) or targetdir

		return exists .. 'ln -s -f ' .. sourcepath .. ' "' .. destpath .. '" || :'
	else
		local targetdir = '${TARGETDIR}'
		if iscygwin then
			sourcepath = os.capture('cygpath -u "' .. sourcepath .. '"')
			targetdir = os.capture('cygpath -u "' .. targetdir .. '"')
		end

		local exists = string.find(sourcepath, '*') and '' or ('test -e ' .. sourcepath .. ' && ')

		destpath = destpath and path.join(targetdir, destpath) or targetdir

		return exists .. 'ln -s -f ' .. sourcepath .. ' "' .. destpath .. '" || :'
	end
end

minko.action.clean = function(directory)
	return 'git clean -X -d -f ' .. directory;
end

minko.action.zip = function(directory, archive)
	if os.is('windows') then
		return '7za a "' .. archive .. '" "' .. path.translate(directory) .. '"'
	else
		return 'zip -r "' .. archive .. '" "' .. directory .. '"'
	end
end

minko.action.optimize = function(file)
	local binary = 'minko-scene-converter'

	local supported = {
		dae = 'scene',
		fbx = 'scene',
		obj = 'scene',
		png = 'texture'
	}

	assert(type(files) == 'table', '`optimize` action expects an array of files')

	local sourceext = path.getextension(file)
	local exportext = minko.package._action['optimize'].supported[sourceext]
	if exportext ~= nil then
		prelinkcommands {
			table.concat({
				minko.package._action['optimize'].binary,
				'-v',
				'-i',
				file,
				'-o',
				file .. '.' .. exportext
			}, ' ')
		}
	end
end
