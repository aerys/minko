minko.action = {}

local function iscygwin()
	return string.startswith(os.getenv('OSTYPE'), 'CYGWIN')
end

local function gettargetdir()
	if os.is('windows') and not iscygwin() then
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

	if iscygwin() then
		filepath = string.gsub(filepath, '([a-z]):', '/cygdrive/%1')
		filepath = string.gsub(filepath, '\\', '/')
	end

	return filepath
end

minko.action.fail = function(target)
	if not target then
		if os.is('windows') and not iscygwin() then
			target = '$(Target)'
		else
			target = '${TARGET}'
		end
	end

	if os.is('windows') and not iscygwin() then
		return 'call "' .. translate(minko.sdk.path('/script/fail.bat')) .. '" "' .. target .. '"'
	else
		return 'sh ' .. minko.sdk.path('/script/fail.sh') .. ' "' .. target .. '"'
	end
end

minko.action.copy = function(sourcepath, destpath, targetdir)
	-- print('minko.action.copy(' .. sourcepath .. ')')

	-- default destpath will be the target directory

	if not targetdir then
		targetdir = gettargetdir()
	end

	if not destpath then
		destpath = path.getname(sourcepath)
	end

	destpath = path.join(targetdir, destpath)

	local destdir = path.getdirectory(destpath)

	if string.find(path.getname(destpath), '*') then
		destpath = path.getdirectory(destpath)
	end

	if os.isdir(sourcepath) and not string.endswith(sourcepath, '/') then
		sourcepath = sourcepath .. '/' -- cp will copy the content of the directory
	end

	if os.is('windows') and not iscygwin() then
		-- print(' -> xcopy /y /i /e "' .. translate(sourcepath) .. '" "' .. translate(destpath) .. '"')

		return 'mkdir "' .. translate(destdir) .. '" & ' ..
			   'xcopy /y /e /i "' .. translate(sourcepath) .. '" "' .. translate(destdir) .. '"'
	else
		if iscygwin() then
			sourcepath = translate(sourcepath)
			targetdir = translate(targetdir)
		end

		-- print(' -> cp -R ' .. sourcepath .. ' "' .. destpath .. '"')

		return 'mkdir -p "' .. destdir .. '"; ' ..
			   'cp -R "' .. sourcepath .. '" "' .. destdir .. '"'
	end
end

minko.action.link = function(sourcepath, destpath)
	local targetdir = gettargetdir()

	if not destpath then
		destpath = path.getname(sourcepath)
	end

	destpath = path.join(targetdir, destpath)

	if os.is('windows') and not iscygwin() then
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

	if os.is('windows') and not iscygwin() then
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
		return '7za a "' .. archive .. '" "' .. translate(directory) .. '"'
	else
		return 'zip -r "' .. archive .. '" "' .. directory .. '"'
	end
end

minko.action.remove = function(filepath)
	local targetdir = gettargetdir()

	if os.is('windows') then
		return 'erase /f /q ' .. translate(path.join(targetdir, filepath))
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
		prelinkcommands {
			table.concat({
				binary,
				'-v',
				'-i',
				file,
				'-o',
				file .. '.' .. exportext
			}, ' ')
		}
	end
end
