minko.action = {}

local function iscygwin()
	return string.startswith(os.getenv('OSTYPE'), 'CYGWIN')
end

local function gettargetdir()
	if os.is('windows') and not iscygwin() then
		if string.startswith(_ACTION, "gmake") then
			return '$(subst /,\\,$(TARGETDIR))'
		else
			return '$(TargetDir)'
		end
	else
		if (_ACTION == "xcode-osx") then
			return '${TARGET_BUILD_DIR}'
		elseif (_ACTION == "xcode-ios") then
			return '${TARGET_BUILD_DIR}/${TARGET_NAME}.app'
		else
			return '${TARGETDIR}'
		end
	end
end

minko.action.fail = function(target)
	if not target then
		if os.is('windows') and not string.startswith(_ACTION, "gmake") then
			target = '$(Target)'
		else
			target = '${TARGET}'
		end
	end

	if os.is('windows') then
		return 'call "' .. path.translate(minko.sdk.path('/tool/win/script/fail.bat')) .. '" "' .. target .. '"'
	else
		return 'bash ' .. minko.sdk.path('/tool/lin/script/fail.sh') .. ' "' .. target .. '"'
	end
end

minko.action.copy = function(sourcepath, destpath)
	-- print('minko.action.copy(' .. sourcepath .. ')')

	-- default destpath will be the target directory

	local targetdir = gettargetdir()

	if os.is('windows') and not iscygwin() then

		sourcepath = path.translate(sourcepath)

		destpath = destpath and path.join(targetdir, destpath) or targetdir

		if os.isdir(sourcepath) then
			destpath = path.join(destpath, path.getbasename(sourcepath))
		end

		-- print(' -> xcopy /y /i /e "' .. path.translate(sourcepath) .. '" "' .. path.translate(destpath) .. '"')

		return 'mkdir "' .. path.translate(path.getdirectory(destpath)) .. '" & ' ..
			   'xcopy /y /e "' .. path.translate(sourcepath) .. '" "' .. path.translate(destpath) .. '"'

	else

		if not destpath then
			destpath = path.getname(sourcepath)
		end

		destpath = path.join(targetdir, destpath)

		-- local destdir = os.isdir(sourcepath) and destpath or path.getdirectory(destpath)

		if os.isdir(sourcepath) and not string.endswith(sourcepath, '/') then
			sourcepath = sourcepath .. '/' -- cp will copy the content of the directory
		end

		if iscygwin() then
			sourcepath = os.capture('cygpath -u "' .. sourcepath .. '"')
			targetdir = os.capture('cygpath -u "' .. targetdir .. '"')
		end

		-- print(' -> cp -R ' .. sourcepath .. ' "' .. destpath .. '"')

		return 'mkdir -p "' .. path.getdirectory(destpath) .. '"; ' ..
			   'cp -R "' .. sourcepath .. '" "' .. destpath .. '"'
	end
end

minko.action.link = function(sourcepath)
	local targetdir = gettargetdir()

	if os.is('windows') and not iscygwin() then
		-- fixme: not needed yet
	else
		local exists = string.find(sourcepath, '*') and '' or ('test -e ' .. sourcepath .. ' && ')

		destpath = destpath and path.join(targetdir, destpath) or targetdir

		return exists .. 'ln -s -f ' .. sourcepath .. ' "' .. destpath .. '" || :'
	end
end

minko.action.embed = function(sourcepath, destpath)
	local targetdir = gettargetdir()
	local preloadfilename = project().name .. '.preload'

	preloadfilename = path.join(targetdir, preloadfilename)

	local bin = (os.is('windows') and not iscygwin()) and '@echo' or 'echo'

	return table.concat({
		bin,
		sourcepath .. '@' .. destpath,
		'>>',
		preloadfilename
	}, ' ')
end

minko.action.unless = function(filepath)
	-- if globbing, always execute command
	if string.find(filepath, '*') then
		return ''
	end

	if os.is('windows') and not iscygwin() then
		return 'if not exist "' .. filepath .. '" '
	else
		return 'test -f ' .. filepath .. ' || '
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

minko.action.remove = function(filepath)
	if os.is('windows') then
		return 'erase /f /q ' .. filepath
	else
		return 'rm -f ' .. filepath
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
