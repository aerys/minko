newoption {
	trigger	= 'no-examples',
	description = 'Disable examples.'
}

newoption {
	trigger	= 'no-tutorials',
	description = 'Disable tutorials.'
}

newoption {
	trigger	= 'no-tests',
	description = 'Disable tests.'
}

newoption {
	trigger = 'dist-dir',
	description = 'Output folder for the redistributable SDK built with the \'dist\' action.'
}

solution "minko"
	MINKO_HOME = path.getabsolute(os.getcwd())
	
	dofile('sdk.lua')

	-- buildable SDK
	MINKO_SDK_DIST = false
	
	include 'framework'
	
	-- plugins
	include 'plugins/lua'
	include 'plugins/angle'
	include 'plugins/assimp'
	include 'plugins/bullet'
	include 'plugins/fx'
	include 'plugins/http-loader'
	include 'plugins/http-worker'
	include 'plugins/jpeg'
	include 'plugins/leap'
	include 'plugins/oculus'
	include 'plugins/offscreen'
	include 'plugins/particles'
	include 'plugins/png'
	include 'plugins/sdl'
	include 'plugins/serializer'
	include 'plugins/webgl'

	-- examples
	if not _OPTIONS['no-examples'] then
		include 'examples/lua-scripts'
		include 'examples/animation'
		include 'examples/assimp'
		include 'examples/cube'
		include 'examples/effect-config'
		include 'examples/frustum'
		include 'examples/jobs'
		include 'examples/leap-motion'
		include 'examples/light'
		include 'examples/line-geometry'
		include 'examples/offscreen'
		include 'examples/picking'
		include 'examples/raycasting'
		include 'examples/serializer'
		include 'examples/sky-box'
		include 'examples/stencil'
		include 'examples/visibility'
		include 'examples/multi-surfaces'
		include 'examples/physics'
		include 'examples/oculus'
		include 'examples/http'
		include 'examples/joystick'
	end

	-- tutorials
	if not _OPTIONS['no-tutorials'] then
		include 'tutorials/basic-material'
		include 'tutorials/camera'
		include 'tutorials/cube'
		include 'tutorials/load-scene-file'
		include 'tutorials/load-scenes'
		include 'tutorials/mouse-camera-rotating'
		include 'tutorials/moving-objects'
		include 'tutorials/moving-objects-keyboard'
		include 'tutorials/resizing'
		include 'tutorials/custom-effect'
		include 'tutorials/custom-materials'
		include 'tutorials/custom-vertex-attributes'
		include 'tutorials/environment-maps'
		include 'tutorials/external-glsl-code'
		include 'tutorials/loading-effects'
		include 'tutorials/model-world-transform'
		include 'tutorials/normal-maps'
		include 'tutorials/phong-material'
		include 'tutorials/post-processing-effect'
		include 'tutorials/specular-maps'
		include 'tutorials/textures'
		include 'tutorials/uber-shaders'
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
		os.mkdir(distDir .. '/framework/bin')
		minko.os.copyfiles('framework/bin', distDir .. '/framework/bin')
		os.mkdir(distDir .. '/framework/include')
		minko.os.copyfiles('framework/include', distDir .. '/framework/include')
		os.mkdir(distDir .. '/framework/lib')
		minko.os.copyfiles('framework/lib', distDir .. '/framework/lib')
		os.mkdir(distDir .. '/framework/asset')
		minko.os.copyfiles('framework/asset', distDir .. '/framework/asset')

		-- skeleton
		os.mkdir(distDir .. '/skeleton')
		minko.os.copyfiles('skeleton', distDir .. '/skeleton')
		
		-- modules
		os.mkdir(distDir .. '/modules')
		minko.os.copyfiles('modules', distDir .. '/modules')
		
		-- -- docs
		-- os.mkdir(distDir .. '/doc')
		-- minko.os.copyfiles('doc/html', distDir .. '/doc')
		
		-- tools
		os.mkdir(distDir .. '/tools/')
		minko.os.copyfiles('tools', distDir .. '/tools')
		
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

			if minko.plugin[pluginName] and minko.plugin[pluginName].dist then
				minko.plugin[pluginName]:dist(dir)
			end

			-- bin
			if os.isdir(basedir .. '/bin') then
				os.mkdir(binDir)
				minko.os.copyfiles(basedir .. '/bin', binDir)
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

		minko.action.zip(distDir, distDir .. '.zip')
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
		minko.action.clean()
	end
}
