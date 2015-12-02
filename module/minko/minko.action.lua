minko.action = {}

local function gettargetdir()
	if os.is('windows') and not os.iscygwin() then
		return '$(TARGETDIR)'
	elseif (_ACTION == "xcode-osx") then
		return '${TARGET_BUILD_DIR}'
	elseif (_ACTION == "xcode-ios") then
		return '${TARGET_BUILD_DIR}/${TARGET_NAME}.app'
	else
		return '${TARGETDIR}'
	end
end

local function translate(filepath)
	filepath = path.translate(filepath)

	if os.is('windows') and not os.iscygwin() and string.startswith(_ACTION, "gmake") then
		filepath = string.gsub(filepath, '%$%(TARGETDIR%)', '$(subst /,\\,$(TARGETDIR))')
	end

	filepath = '"' .. filepath  .. '"'

	return filepath
end

minko.action.fail = function(target)
	if not target then
		if os.is('windows') and not os.iscygwin() then
			target = '$(Target)'
		else
			target = '${TARGET}'
		end
	end

	if os.is('windows') and not os.iscygwin() then
		return 'call "' .. path.translate(minko.sdk.path('/script/fail.bat')) .. '" "' .. target .. '"'
	else
		return 'sh ' .. minko.sdk.path('/script/fail.sh') .. ' "' .. target .. '"'
	end
end

minko.action.copy = function(sourcepath, destpath, targetdir)
	-- print('minko.action.copy(' .. sourcepath .. ')')

	-- default destpath will be the target directory

	if not string.find(path.getname(sourcepath), '*') and
	   not os.isfile(sourcepath) and not os.isdir(sourcepath) then
		return ''
	end

	if not targetdir then
		targetdir = gettargetdir()
	end

	if not destpath then
		destpath = path.getname(sourcepath)
	end

	if string.endswith(destpath, '/') then
		destpath = path.join(targetdir, destpath) .. '/'
	else
		destpath = path.join(targetdir, destpath)
	end
	if string.find(path.getname(destpath), '*') then
		destpath = path.getdirectory(destpath)
	end

	local destdir = destpath

	if os.is('windows') and not os.iscygwin() then
		-- print(' -> xcopy /y /i /e "' .. translate(sourcepath) .. '" "' .. translate(destdir) .. '"')

		if destpath ~= gettargetdir() and not os.isdir(sourcepath) then
			destdir = path.getdirectory(destpath)
		end

		local xcopyoptions = '/y /i'

		if os.isdir(sourcepath) then
			xcopyoptions = xcopyoptions .. ' /e'
		end

		return 'mkdir ' .. translate(destdir) .. ' & ' ..
			   'xcopy ' .. xcopyoptions .. ' ' .. translate(sourcepath) .. ' ' .. translate(destdir)
	else
		if destpath ~= gettargetdir() and not string.endswith(destpath, '/') then
			destdir = path.getdirectory(destpath)
		end

		if not os.is("macosx") then
			if os.isdir(sourcepath) and not string.endswith(sourcepath, '/') then
				sourcepath = sourcepath .. '/' -- cp will copy the content of the directory
			end
		end

		if os.iscygwin() then
			sourcepath = path.cygpath(sourcepath)
			targetdir = path.cygpath(targetdir)
		end

		-- print(' -> cp -R ' .. sourcepath .. ' "' .. destdir .. '"')

		return 'mkdir -p ' .. destdir .. '; ' ..
			   'cp -R ' .. sourcepath .. ' ' .. destdir
	end
end

minko.action.link = function(sourcepath, destpath)
	local targetdir = gettargetdir()

	if not destpath then
		destpath = path.getname(sourcepath)
	end

	destpath = path.join(targetdir, destpath)

	if os.is('windows') and not os.iscygwin() then
		if os.isdir(sourcepath) then
			return 'mklink /d "' .. destpath .. '" "' .. sourcepath .. '"'
		else
			return 'mklink "' .. destpath .. '" "' .. sourcepath .. '"'
		end
	else
		return 'ln -s -f "' .. sourcepath .. '"" "' .. destpath .. '"'
	end
end

minko.action.embed = function(sourcepath, destpath)
	return minko.action.copy(sourcepath, destpath, path.join(gettargetdir(), 'embed'))
end

minko.action.unless = function(filepath)
	-- if globbing, always execute command
	if string.find(filepath, '*') then
		return ''
	end

	if os.is('windows') and not os.iscygwin() then
		return 'if not exist "' .. filepath .. '" '
	else
		return 'test -f ' .. filepath .. ' || '
	end
end

minko.action.clean = function(directory)
	return 'git clean -X -d -f ' .. directory
end

minko.action.zip = function(directory, archive)
	if os.is('windows') then
		return '7za a "' .. archive .. '" "' .. path.translate(directory) .. '"'
	else
		return 'zip -r "' .. archive .. '" "' .. directory .. '"'
	end
end

minko.action.exclude = function(filepath)
	local targetdir = gettargetdir()

	if os.is('windows') then
		return 'erase /f /q ' .. path.translate(path.join(targetdir, filepath))
	else
		return 'rm -f ' .. path.join(targetdir, filepath)
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
	local exportext = supported[sourceext]

	if exportext ~= nil then
		return table.concat({
			binary,
			'-v',
			'-i',
			file,
			'-o',
			file .. '.' .. exportext
		}, ' ')
	else
		return ''
	end
end

minko.action.cpjf = function(absoluteSrcDir, relativeDestDir)
	local scriptLocation = MINKO_HOME .. '/script/cpjf.sh';

	if string.startswith(os.getenv('OSTYPE'), 'CYGWIN') then
		scriptLocation = os.capture('cygpath -u "' .. scriptLocation .. '"')
		absoluteSrcDir = os.capture('cygpath -u "' .. absoluteSrcDir .. '"')
	end

	return 'bash ' .. scriptLocation .. ' ' .. absoluteSrcDir .. ' ' .. relativeDestDir .. ' || ' .. minko.action.fail()
end

minko.action.buildandroid = function()
	local minkoHome = MINKO_HOME

	if string.startswith(os.getenv('OSTYPE'), 'CYGWIN') then
		minkoHome = os.capture('cygpath -u "' .. minkoHome .. '"')
	end

	return 'bash ' .. minkoHome .. '/script/build_android.sh ${TARGET} || ' .. minko.action.fail()
end
