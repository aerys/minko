-- set MINKO_HOME
if os.getenv('MINKO_HOME') then
	MINKO_HOME = os.getenv('MINKO_HOME');
else
	MINKO_HOME = path.getabsolute(os.getcwd())
end

if not os.isfile(MINKO_HOME .. '/sdk.lua') then
	printf('Running premake4 from outside Minko SDK. You must define the environment variable MINKO_HOME.')
end

printf('Minko SDK home directory: ' .. MINKO_HOME)

-- import build system utilities
dofile(MINKO_HOME .. '/tools/all/lib/minko.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.sdk.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.os.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.path.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.plugin.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.vs.lua')
dofile(MINKO_HOME .. '/tools/all/lib/minko.project.lua')

-- add new platforms
minko.sdk.newplatform {
	name = 'emscripten',
	description = 'Emscripten C++ to JS toolchain',
	gcc = {
		cc = MINKO_HOME .. '/tools/lin/bin/emcc.sh',
		cxx = MINKO_HOME .. '/tools/lin/bin/em++.sh',
		ar = MINKO_HOME .. '/tools/lin/bin/emar.sh',
		cppflags = '-MMD -DEMSCRIPTEN'
	}
}

minko.sdk.newplatform {
	name = 'clang',
	description = 'Clang',
	gcc = {
		cc = 'clang',
		cxx = 'clang++',
		ar = 'ar',
		cppflags = '-stdlib=libc++',
		ldflags = '-stdlib=libc++'
	}
}

minko.sdk.newplatform {
	name = 'gcc',
	description = 'GCC',
	gcc = {
		cc = 'gcc',
		cxx = MINKO_HOME .. '/tools/lin/bin/g++-ld.sh',
		ar = 'ar',
		cppflags = '-MMD',
		ldflags = ''
	}
}

-- options
if _OPTIONS.platform then
	print('Selected target platform: ' .. _OPTIONS['platform'])
    -- overwrite the native platform with the options::platform
    premake.gcc.platforms['Native'] = premake.gcc.platforms[_OPTIONS.platform]
end

newoption {
	trigger	= "no-stencil",
	description = "Disable all stencil operations."
}
if _OPTIONS["no-stencil"] then
	defines { "MINKO_NO_STENCIL" }
end

-- actions
newaction {
	trigger		= "dist",
	description	= "Generate the distributable version of the Minko SDK.",
	execute		= function()
	
		os.rmdir('dist')
		
		os.mkdir('dist/framework')
		os.mkdir('dist/framework/bin/release')
		os.mkdir('dist/framework/bin/debug')
		os.mkdir('dist/framework/include')
		os.mkdir('dist/plugin')
		
		-- skeleton
		os.mkdir('dist/skeleton')
		minko.os.copyfiles('skeleton', 'dist/skeleton')
		
		-- docs
		os.mkdir('dist/doc')
		minko.os.copyfiles('doc/html', 'dist/doc')
		
		-- tools
		os.mkdir('dist/tool/' .. os.get())
		
		-- deps
		os.mkdir('dist/deps')
		minko.os.copyfiles('deps', 'dist/deps')
		
		for i, v in ipairs(solution()['projects']) do
			print(v['basedir'])

			-- plugins
			if string.startswith(v['name'], "plugin-") then
				local dir = 'dist/plugin/' .. path.getbasename(v['basedir'])
				local binDir = dir .. '/bin'
				
				os.mkdir(dir)
				os.copyfile(v['basedir'] .. '/plugin.lua', dir .. '/plugin.lua')
				
				-- bin
				os.mkdir(binDir .. '/debug/' .. os.get())
				os.mkdir(binDir .. '/release/' .. os.get())
				minko.os.copyfiles(v['basedir'] .. '/bin/debug', binDir .. '/debug/' .. os.get())
				minko.os.copyfiles(v['basedir'] .. '/bin/release', binDir .. '/release/' .. os.get())
				
				-- includes
				if os.isdir(v['basedir'] .. '/include') then
					os.mkdir(dir .. '/include')
					minko.os.copyfiles(v['basedir'] .. '/include', dir .. '/include')
				end
				
				-- assets
				if os.isdir(v['basedir'] .. '/asset') then
					os.mkdir(dir .. '/asset')
					minko.os.copyfiles(v['basedir'] .. '/asset', dir .. '/asset')
				end
			end
		end
	end
}
