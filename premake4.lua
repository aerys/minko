solution "minko"
	configurations { "debug", "release" }

	dofile('sdk.lua')
	
	include 'framework'
	
	-- plugins
	include 'plugins/jpeg'
	include 'plugins/png'
	include 'plugins/mk'
	include 'plugins/bullet'
	include 'plugins/particles'
	include 'plugins/sdl'
	include 'plugins/angle'
	include 'plugins/fx'
	include 'plugins/assimp'
	include 'plugins/offscreen'
	if _OPTIONS["platform"] == "emscripten" then
		include 'plugins/webgl'
	end

	-- tests
	if _ACTION ~= "vs2010" and _OPTIONS["platform"] ~= "emscripten" then
		include 'tests'
	end

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
		
		os.copyfile('sdk.lua', 'dist/sdk.lua')

		-- skeleton
		os.mkdir('dist/skeleton')
		minko.os.copyfiles('skeleton', 'dist/skeleton')
		
		-- docs
		os.mkdir('dist/doc')
		minko.os.copyfiles('doc/html', 'dist/doc')
		
		-- tools
		os.mkdir('dist/tools/')
		minko.os.copyfiles('tools', 'dist/tools')
		
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
