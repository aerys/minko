require 'color'

newoption {
	trigger	= 'no-examples',
	description = 'Disable examples.'
}

newoption {
	trigger	= 'no-tests',
	description = 'Disable tests.'
}

newoption {
	trigger = 'dist-dir',
	description = 'Output folder for the redistributable SDL built with the \'dist\' action.'
}

solution "minko"
	configurations { "debug", "release" }
	platforms { "linux", "win", "osx", "html5", "ios", "android" }

	MINKO_HOME = path.getabsolute(os.getcwd())

	dofile('sdk.lua')

	configuration { "osx" }
		system "macosx"

	configuration { "html5" }
		system "emscripten"

	configuration { "android"}
		system "android"

	configuration {}

	-- buildable SDK
	MINKO_SDK_DIST = false
	
	include 'framework'
	
	-- plugins
	include 'plugins/jpeg'
	include 'plugins/png'
	include 'plugins/mk'
	include 'plugins/bullet'
	include 'plugins/particles'
	include 'plugins/webgl'
	include 'plugins/sdl'
	include 'plugins/angle'
	include 'plugins/fx'
	include 'plugins/assimp'
	include 'plugins/offscreen'

	-- examples
	if not _OPTIONS['no-examples'] then
		include('examples/assimp')
		include('examples/cube')
		include('examples/effect-config')
		include('examples/light')
		include('examples/raycasting')
		include('examples/sponza')
		include('examples/stencil')
	end

	-- tests
	if not _OPTIONS['no-tests'] then
		include 'tests'
	end

newaction {
	trigger		= 'dist',
	description	= 'Generate the distributable version of the Minko SDK.',
	execute		= function()
	
		local distDir = 'dist'
		
		if _OPTIONS['dist-dir'] then
			distDir = _OPTIONS['dist-dir']
		end
		
		os.rmdir(distDir)

		os.mkdir(distDir)
		os.copyfile('sdk.lua', distDir .. '/sdk.lua')

		print("Packaging core framework...")
		
		-- framework
		os.mkdir(distDir .. '/framework')
		os.mkdir(distDir .. '/framework/bin/release')
		minko.os.copyfiles('framework/bin/release', distDir .. '/framework/bin/release')
		os.mkdir(distDir .. '/framework/bin/debug')
		minko.os.copyfiles('framework/bin/debug', distDir .. '/framework/bin/debug')
		os.mkdir(distDir .. '/framework/include')
		minko.os.copyfiles('framework/include', distDir .. '/framework/include')
		os.mkdir(distDir .. '/framework/effect')
		minko.os.copyfiles('framework/effect', distDir .. '/framework/effect')

		-- skeleton
		os.mkdir(distDir .. '/skeleton')
		minko.os.copyfiles('skeleton', distDir .. '/skeleton')
		
		-- docs
		os.mkdir(distDir .. '/doc')
		minko.os.copyfiles('doc/html', distDir .. '/doc')
		
		-- tools
		os.mkdir(distDir .. '/tools/')
		minko.os.copyfiles('tools', distDir .. '/tools')
		
		-- deps
		os.mkdir(distDir .. '/deps')
		minko.os.copyfiles('deps', distDir .. '/deps')
		
		-- plugins
		local plugins = os.matchdirs('plugins/*')

		os.mkdir(distDir .. '/plugins')
		for i, basedir in ipairs(plugins) do
			local pluginName = path.getbasename(basedir)

			print('Packaging plugin "' .. pluginName .. '"...')

			-- plugins
			local dir = distDir .. '/plugins/' .. path.getbasename(basedir)
			local binDir = dir .. '/bin'

			-- plugin.lua
			assert(os.isfile(basedir .. '/plugin.lua'), 'missing plugin.lua')			
			os.mkdir(dir)
			os.copyfile(basedir .. '/plugin.lua', dir .. '/plugin.lua')

			dofile(dir .. '/plugin.lua')
			if minko.plugin[pluginName] and minko.plugin[pluginName].dist then
				minko.plugin[pluginName]:dist(dir)
			end

			if solution()['projects']['minko-plugin-' .. pluginName] then
				-- bin
				assert(os.isdir(basedir .. '/bin/debug'), 'missing debug folder')
				assert(os.isdir(basedir .. '/bin/release'), 'missing release folder')

				os.mkdir(binDir .. '/debug')
				os.mkdir(binDir .. '/release')
				minko.os.copyfiles(basedir .. '/bin/debug', binDir .. '/debug')
				minko.os.copyfiles(basedir .. '/bin/release', binDir .. '/release')
			end

			-- includes
			if os.isdir(basedir .. '/include') then
				os.mkdir(dir .. '/include')
				minko.os.copyfiles(basedir .. '/include', dir .. '/include')
			end
			
			-- assets
			if os.isdir(basedir .. '/asset') then
				os.mkdir(dir .. '/asset')
				minko.os.copyfiles(basedir .. '/asset', dir .. '/asset')
			end
		end
	end
}

newaction {
	trigger			= "doxygen",
	description		= "Create developer reference.",
	execute			= function()
		os.execute("doxygen")
	end
}

newaction {
	trigger			= "clean",
	description		= "Remove generated files.",
	execute			= function()
	local prjs = solution().projects
		for i, prj in ipairs(prjs) do
			os.rmdir(prj.basedir .. "/bin")
			os.rmdir(prj.basedir .. "/obj")
		end
		os.rmdir(solution().basedir .. "/doc")
	end
}
