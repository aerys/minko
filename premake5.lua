newoption {
	trigger	= 'no-examples',
	description = 'Disable examples.'
}

newoption {
	trigger	= 'no-tutorial',
	description = 'Disable tutorial.'
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

	-- tutorial
	if not _OPTIONS['no-tutorial'] then
		include 'tutorial/01-hello-cube'
		include 'tutorial/02-handle-canvas-resizing'
		include 'tutorial/03-rotating-the-camera-around-an-object-with-the-mouse'
		include 'tutorial/04-moving-objects'
		include 'tutorial/05-moving-objects-with-the-keyboard'
		include 'tutorial/06-load-3d-files'
		include 'tutorial/07-loading-scene-files'
		include 'tutorial/08-my-first-script'
		include 'tutorial/09-scripting-mouse-inputs'
		include 'tutorial/10-working-with-the-basic-material'
		include 'tutorial/11-working-with-the-phong-material'
		include 'tutorial/12-working-with-normal-maps'
		include 'tutorial/13-working-with-environment-maps'
		include 'tutorial/14-working-with-specular-maps'
		include 'tutorial/15-loading-and-using-textures'
		include 'tutorial/16-loading-effects'
		include 'tutorial/17-creating-a-custom-effect'
		include 'tutorial/18-creating-custom-materials'
		include 'tutorial/19-binding-the-model-to-world-transform'
		include 'tutorial/20-binding-the-camera'
		include 'tutorial/21-authoring-uber-shaders'
		include 'tutorial/22-creating-a-simple-post-processing-effect'
		-- include 'tutorial/23-using-external-glsl-code-in-effect-files'
		include 'tutorial/24-working-with-custom-vertex-attributes'
		include 'tutorial/25-working-with-ambient-lights'
		include 'tutorial/26-working-with-directional-lights'
		include 'tutorial/27-working-with-point-lights'
		include 'tutorial/28-working-with-spot-lights'
		include 'tutorial/29-hello-falling-cube'
		include 'tutorial/30-applying-antialiasing-effect'
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
