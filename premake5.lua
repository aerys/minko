newoption {
	trigger	= 'no-example',
	description = 'Disable examples.'
}

newoption {
	trigger	= 'no-tutorial',
	description = 'Disable tutorial.'
}

newoption {
	trigger	= 'no-test',
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

	-- plugin
	include 'plugin/lua'
	include 'plugin/angle'
	include 'plugin/assimp'
	include 'plugin/devil'
	include 'plugin/bullet'
	include 'plugin/fx'
	include 'plugin/http-loader'
	include 'plugin/http-worker'
	include 'plugin/jpeg'
	include 'plugin/leap'
	include 'plugin/oculus'
	include 'plugin/offscreen'
	include 'plugin/particles'
	include 'plugin/png'
	include 'plugin/sdl'
	include 'plugin/serializer'
	include 'plugin/webgl'

	-- example
	if not _OPTIONS['no-example'] then
		include 'example/lua-scripts'
		include 'example/assimp'
		include 'example/cube'
		include 'example/devil'
		include 'example/effect-config'
		include 'example/fog'
		include 'example/frustum'
		include 'example/jobs'
		include 'example/leap-motion'
		include 'example/light'
		include 'example/line-geometry'
		include 'example/offscreen'
		include 'example/picking'
		include 'example/raycasting'
		include 'example/serializer'
		include 'example/sky-box'
		include 'example/stencil'
		include 'example/visibility'
		include 'example/multi-surfaces'
		include 'example/physics'
		include 'example/oculus'
		include 'example/http'
		include 'example/joystick'
	end

	-- test
	if not _OPTIONS['no-test'] then
		include 'test'
	end

newaction {
	trigger		= 'dist',
	description	= 'Generate the distributable version of the Minko SDK.',
	execute		= function()
	
		-- print("Building documentation...")
		-- os.execute("doxygen doc/Doxyfile")

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
		
		-- module
		os.mkdir(distDir .. '/module')
		minko.os.copyfiles('module', distDir .. '/module')
		
		-- -- doc
		-- os.mkdir(distDir .. '/doc')
		-- minko.os.copyfiles('doc/html', distDir .. '/doc')
		
		-- tool
		os.mkdir(distDir .. '/tool/')
		minko.os.copyfiles('tool', distDir .. '/tool')
		
		-- plugin
		local plugins = os.matchdirs('plugin/*')

		os.mkdir(distDir .. '/plugin')
		for i, basedir in ipairs(plugins) do
			local pluginName = path.getbasename(basedir)

			print('Packaging plugin "' .. pluginName .. '"...')

			-- plugins
			local dir = distDir .. '/plugin/' .. path.getbasename(basedir)
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
	trigger			= "doc",
	description		= "Create developer reference.",
	execute			= function()
		os.execute("doxygen doc/Doxyfile")
	end
}

newaction {
	trigger			= "clean",
	description		= "Remove generated files.",
	execute			= function()
		minko.action.clean()
	end
}
